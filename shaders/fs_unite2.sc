$input v_uv, v_rectParams, v_patternParams, v_scroll, v_shadowBorder, v_uvRange, v_fillColor, v_borderColor, v_shadowColor

#include <bgfx_shader.sh>

SAMPLER2D(s_palette, 0);
SAMPLER2D(s_widths, 1);

// モード定数
#define MODE_FILL             0.0
#define MODE_GRADIENT         1.0
#define MODE_STRIPE           2.0
#define MODE_CHECKER          3.0
#define MODE_GRADIENT_CHECKER 4.0
#define MODE_IMAGE            5.0
#define MODE_PAGE_CURL        6.0
#define MODE_RAW_IMAGE        7.0
#define MODE_END              8.0

// 旧: 均一radius版（PageCurl影用に残す）
float sdRoundRect(vec2 p, vec2 b, float r)
{
    vec2 q = abs(p) - (b - vec2(r, r));
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}

// per-corner radius版 (Inigo Quilez)
// r = vec4(topRight, bottomRight, bottomLeft, topLeft)
float sdRoundBox(vec2 p, vec2 b, vec4 r)
{
    r.xy = (p.x > 0.0) ? r.xy : r.wz;
    r.x  = (p.y > 0.0) ? r.x  : r.y;
    float rr = r.x;
    vec2 q = abs(p) - (b - vec2(rr, rr));
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - rr;
}

// half16 アンパック (floatBitsToUint の上位/下位16bit)
float unpackHalfHi(uint packed) {
    uint h = (packed >> 16u) & 0xFFFFu;
    uint sign = (h & 0x8000u) << 16u;
    uint exponent = (h >> 10u) & 0x1Fu;
    uint mantissa = h & 0x3FFu;
    if (exponent == 0u) return 0.0;
    uint f = sign | ((exponent + 112u) << 23u) | (mantissa << 13u);
    return uintBitsToFloat(f);
}

float unpackHalfLo(uint packed) {
    uint h = packed & 0xFFFFu;
    uint sign = (h & 0x8000u) << 16u;
    uint exponent = (h >> 10u) & 0x1Fu;
    uint mantissa = h & 0x3FFu;
    if (exponent == 0u) return 0.0;
    uint f = sign | ((exponent + 112u) << 23u) | (mantissa << 13u);
    return uintBitsToFloat(f);
}

uniform vec4 u_param1;

void main()
{
    vec2 p = v_uv * v_rectParams.zw;
    vec2 resolution = v_rectParams.zw;
    float mode = u_param1.x;
    float blendMode = u_param1.y; // 0=premultiplied, 1=overwrite
    
    // v_patternParams:
    //   .xy: (未使用)
    //   .z:  angle / uvMin.x / curlAngle
    //   .w:  (未使用) / uvMin.y / curlRadius
    float u_angle = v_patternParams.z;
    float curlRadius = v_patternParams.w;
    
    // v_scroll:
    //   Fill/Image/Pattern: scrollX, scrollY, pack(rTL,rTR), pack(rBR,rBL)
    //   PageCurl: uvSize.x, uvSize.y, progress, aa
    vec2 scrollXY = v_scroll.xy;

    // v_shadowBorder: shadowX, shadowY, pack(shadowBlur,aa), borderWidth
    vec2 shadowOffset = v_shadowBorder.xy;
    uint sbPacked = floatBitsToUint(v_shadowBorder.z);
    float shadowBlur = unpackHalfHi(sbPacked);
    float aa = unpackHalfLo(sbPacked);
    float borderWidth = v_shadowBorder.w;

    // per-corner radius アンパック (PageCurl以外)
    vec4 cornerRadii = vec4(0.0, 0.0, 0.0, 0.0); // TR, BR, BL, TL
    float radius = 0.0;  // PageCurl用 progress / 影の均一radius用
    
    // v_uvRange:
    //   .xy: colorCount, dataOffset (パターン) / uvMin.xy (PageCurl、整数)
    //   .zw: backUVMin.xy (ページめくりのみ、整数)
    //   ※ Imageモードでは v_uvRange 未使用
    float u_colorCount = v_uvRange.x;
    float dataOffset = v_uvRange.y;
    vec2 backUVMinInt = v_uvRange.zw;
    
    float totalSize = 1024.0;
    
    // ============================================================
    // パターン色の決定
    // ============================================================
    
    vec4 patternCol = v_fillColor;

    // ----------------------------------------------------------
    // Fill (初期値をそのまま使用)
    // ----------------------------------------------------------
    if (mode < MODE_GRADIENT) {
        // patternCol は既に v_fillColor で初期化済み
    }
    // ----------------------------------------------------------
    // Gradient (線形補間)
    // ----------------------------------------------------------
    else if (mode < MODE_STRIPE) {
        float widthU = (dataOffset + u_colorCount - 0.5) / totalSize;
        float u_totalWidth = texture2D(s_widths, vec2(widthU, 0.5)).r;
        u_totalWidth = max(u_totalWidth, 0.001);
        
        float s = sin(u_angle);
        float cs = cos(u_angle);
        vec2 r = vec2(p.x * cs - p.y * s, p.x * s + p.y * cs);
        
        vec2 scrollRot = vec2(
            scrollXY.x * cs + scrollXY.y * s,
           -scrollXY.x * s + scrollXY.y * cs
        );
        
        float axis = r.x + scrollRot.x;
        float m = mod(axis, u_totalWidth);
        m = min(m, u_totalWidth - 0.001);
        
        float prevLimit = 0.0;
        float found = 0.0;
        patternCol = vec4(0.0, 0.0, 0.0, 0.0);
        
        for (float i = 0.0; i < 32.0; i += 1.0) {
            if (found < 0.5 && i < u_colorCount) {
                float wU = (dataOffset + i + 0.5) / totalSize;
                float limit = texture2D(s_widths, vec2(wU, 0.5)).r;
                
                if (m < limit) {
                    float segmentWidth = limit - prevLimit;
                    float t = (segmentWidth > 0.001) ? clamp((m - prevLimit) / segmentWidth, 0.0, 1.0) : 0.0;
                    
                    float pU_A = (dataOffset + i + 0.5) / totalSize;
                    float nextIdx = mod(i + 1.0, u_colorCount);
                    float pU_B = (dataOffset + nextIdx + 0.5) / totalSize;
                    
                    vec4 colA = texture2D(s_palette, vec2(pU_A, 0.5));
                    vec4 colB = texture2D(s_palette, vec2(pU_B, 0.5));
                    patternCol = mix(colA, colB, t);
                    found = 1.0;
                }
                prevLimit = limit;
            }
        }
        
        if (found < 0.5) {
            float pU = (dataOffset + 0.5) / totalSize;
            patternCol = texture2D(s_palette, vec2(pU, 0.5));
        }
    }
    // ----------------------------------------------------------
    // Stripe (補間なし)
    // ----------------------------------------------------------
    else if (mode < MODE_CHECKER) {
        float widthU = (dataOffset + u_colorCount - 0.5) / totalSize;
        float u_totalWidth = texture2D(s_widths, vec2(widthU, 0.5)).r;
        u_totalWidth = max(u_totalWidth, 0.001);
        
        float s = sin(u_angle);
        float cs = cos(u_angle);
        vec2 r = vec2(p.x * cs - p.y * s, p.x * s + p.y * cs);
        
        vec2 scrollRot = vec2(
            scrollXY.x * cs + scrollXY.y * s,
           -scrollXY.x * s + scrollXY.y * cs
        );
        
        float axis = r.x + scrollRot.x;
        float m = mod(axis, u_totalWidth);
        
        float idx = 0.0;
        float found = 0.0;
        
        for (float i = 0.0; i < 32.0; i += 1.0) {
            float inRange = step(0.0, u_colorCount - i - 1.0);
            float wU = (dataOffset + i + 0.5) / totalSize;
            float limit = texture2D(s_widths, vec2(wU, 0.5)).r;
            
            float isHit = (1.0 - found) * inRange * step(m, limit);
            idx += isHit * i;
            found += isHit;
        }
        
        if (found < 0.5) {
            idx = 0.0;
        }
        
        float pU = (dataOffset + idx + 0.5) / totalSize;
        patternCol = texture2D(s_palette, vec2(pU, 0.5));
    }
    // ----------------------------------------------------------
    // Checker (補間なし)
    // ----------------------------------------------------------
    else if (mode < MODE_GRADIENT_CHECKER) {
        float widthU = (dataOffset + u_colorCount - 0.5) / totalSize;
        float u_totalWidth = texture2D(s_widths, vec2(widthU, 0.5)).r;
        u_totalWidth = max(u_totalWidth, 0.001);
        
        float s = sin(u_angle);
        float cs = cos(u_angle);
        vec2 r = vec2(p.x * cs - p.y * s, p.x * s + p.y * cs);
        
        vec2 scrollRot = vec2(
            scrollXY.x * cs + scrollXY.y * s,
           -scrollXY.x * s + scrollXY.y * cs
        );
        
        float mx = mod(r.x + scrollRot.x, u_totalWidth);
        float my = mod(r.y + scrollRot.y, u_totalWidth);
        
        // X軸のインデックス
        float idxX = 0.0;
        float foundX = 0.0;
        for (float i = 0.0; i < 32.0; i += 1.0) {
            float inRange = step(0.0, u_colorCount - i - 1.0);
            float wU = (dataOffset + i + 0.5) / totalSize;
            float limit = texture2D(s_widths, vec2(wU, 0.5)).r;
            
            float isHit = (1.0 - foundX) * inRange * step(mx, limit);
            idxX += isHit * i;
            foundX += isHit;
        }
        
        // Y軸のインデックス
        float idxY = 0.0;
        float foundY = 0.0;
        for (float j = 0.0; j < 32.0; j += 1.0) {
            float inRange = step(0.0, u_colorCount - j - 1.0);
            float wU = (dataOffset + j + 0.5) / totalSize;
            float limit = texture2D(s_widths, vec2(wU, 0.5)).r;
            
            float isHit = (1.0 - foundY) * inRange * step(my, limit);
            idxY += isHit * j;
            foundY += isHit;
        }
        
        float idx = mod(idxX + idxY, u_colorCount);
        float pU = (dataOffset + idx + 0.5) / totalSize;
        patternCol = texture2D(s_palette, vec2(pU, 0.5));
    }
    // ----------------------------------------------------------
    // GradientChecker (バイリニア補間)
    // ----------------------------------------------------------
    else if (mode < MODE_IMAGE) {
        float widthU = (dataOffset + u_colorCount - 0.5) / totalSize;
        float u_totalWidth = texture2D(s_widths, vec2(widthU, 0.5)).r;
        u_totalWidth = max(u_totalWidth, 0.001);
        
        float s = sin(u_angle);
        float cs = cos(u_angle);
        vec2 r = vec2(p.x * cs - p.y * s, p.x * s + p.y * cs);
        
        vec2 scrollRot = vec2(
            scrollXY.x * cs + scrollXY.y * s,
           -scrollXY.x * s + scrollXY.y * cs
        );
        
        float mx = mod(r.x + scrollRot.x, u_totalWidth);
        float my = mod(r.y + scrollRot.y, u_totalWidth);
        
        // X軸：インデックスとセグメント内位置
        float idxX = 0.0;
        float tX = 0.0;
        float foundX = 0.0;
        float prevX = 0.0;
        
        for (float i = 0.0; i < 32.0; i += 1.0) {
            float inRange = step(0.0, u_colorCount - i - 1.0);
            float wU = (dataOffset + i + 0.5) / totalSize;
            float limit = texture2D(s_widths, vec2(wU, 0.5)).r;
            
            float isHit = (1.0 - foundX) * inRange * step(mx, limit);
            idxX += isHit * i;
            float segWidth = max(limit - prevX, 0.001);
            tX += isHit * (mx - prevX) / segWidth;
            foundX += isHit;
            prevX = limit;
        }
        
        // Y軸：インデックスとセグメント内位置
        float idxY = 0.0;
        float tY = 0.0;
        float foundY = 0.0;
        float prevY = 0.0;
        
        for (float j = 0.0; j < 32.0; j += 1.0) {
            float inRange = step(0.0, u_colorCount - j - 1.0);
            float wU = (dataOffset + j + 0.5) / totalSize;
            float limit = texture2D(s_widths, vec2(wU, 0.5)).r;
            
            float isHit = (1.0 - foundY) * inRange * step(my, limit);
            idxY += isHit * j;
            float segWidth = max(limit - prevY, 0.001);
            tY += isHit * (my - prevY) / segWidth;
            foundY += isHit;
            prevY = limit;
        }
        
        tX = clamp(tX, 0.0, 1.0);
        tY = clamp(tY, 0.0, 1.0);
        
        // 4コーナーのインデックス（チェッカー: X + Y）
        float nextX = mod(idxX + 1.0, u_colorCount);
        float nextY = mod(idxY + 1.0, u_colorCount);
        
        float idx00 = mod(idxX + idxY, u_colorCount);
        float idx10 = mod(nextX + idxY, u_colorCount);
        float idx01 = mod(idxX + nextY, u_colorCount);
        float idx11 = mod(nextX + nextY, u_colorCount);
        
        // 4色を取得
        vec4 col00 = texture2D(s_palette, vec2((dataOffset + idx00 + 0.5) / totalSize, 0.5));
        vec4 col10 = texture2D(s_palette, vec2((dataOffset + idx10 + 0.5) / totalSize, 0.5));
        vec4 col01 = texture2D(s_palette, vec2((dataOffset + idx01 + 0.5) / totalSize, 0.5));
        vec4 col11 = texture2D(s_palette, vec2((dataOffset + idx11 + 0.5) / totalSize, 0.5));
        
        // バイリニア補間
        vec4 colTop = mix(col00, col10, tX);
        vec4 colBot = mix(col01, col11, tX);
        patternCol = mix(colTop, colBot, tY);
    }
    // ----------------------------------------------------------
    // Image
    // v_patternParams.z: uvMin.x, v_patternParams.w: uvMin.y
    // v_scroll.xy: uvMax.x, uvMax.y
    // ※ packなし、float精度
    // ----------------------------------------------------------
    else if (mode < MODE_PAGE_CURL) {
        vec2 uvMin = vec2(u_angle, curlRadius);  // v_patternParams.zw
        vec2 uvMax = scrollXY;                    // v_scroll.xy
        vec2 texUV = mix(uvMin, uvMax, clamp(v_uv, 0.0, 1.0));
        patternCol = texture2D(s_palette, texUV) * v_fillColor;
    }
    // ----------------------------------------------------------
    // PageCurl
    // v_patternParams.z: curlAngle, v_patternParams.w: curlRadius
    // v_scroll: uvSize.x, uvSize.y, progress, aa
    // v_uvRange: uvMin.xy(int), backUVMin.xy(int)
    // ----------------------------------------------------------
    else if (mode < MODE_RAW_IMAGE) {
        // 16bitパック値を小数に変換
        vec2 uvMin = v_uvRange.xy / 65535.0;
        vec2 backUVMin = backUVMinInt / 65535.0;
        
        float curlAngle = u_angle;                   // v_patternParams.z
        float curlRadiusRaw = curlRadius;            // v_patternParams.w
        
        vec2 uvSize = scrollXY;                      // v_scroll.xy
        float progress = clamp(v_scroll.z, 0.0, 1.0); // v_scroll.z (PageCurlはfloat32のまま)
        // aa は v_shadowBorder.z からhalf16アンパック済み
        
        bool isSpread = curlRadiusRaw < 0.0;
        
        // 折り目位置
        float foldX = 1.0 - progress;
        float t = 1.0 - progress;
        float effectiveAngle = curlAngle * t * t;
        float sa = sin(effectiveAngle);
        foldX += sa * (v_uv.y - 0.5) * 0.4 * t;
        
        if (isSpread) {
            foldX = max(foldX, 0.5);
        }
        
        bool isFlippedPage = false;
        
        if (v_uv.x > foldX) {
            // 表面（まだめくられていない部分）
            vec2 texUV = uvMin + v_uv * uvSize;
            patternCol = texture2D(s_widths, texUV) * v_fillColor;
        }
        else {
            float backSrcX = 2.0 * foldX - v_uv.x;
            
            if (backSrcX >= 0.0 && backSrcX <= 1.0) {
                // めくった紙の裏面
                float texX = 1.0 - backSrcX;
                vec2 texUV = backUVMin + vec2(texX, v_uv.y) * uvSize;
                patternCol = texture2D(s_widths, texUV) * v_fillColor;
                isFlippedPage = true;
                
                if (isSpread) {
                    float brightness = 0.9 + 0.1 * progress;
                    patternCol.rgb *= brightness;
                }
                else {
                    patternCol.rgb *= 0.9;
                    
                    float distFromFold = foldX - v_uv.x;
                    if (distFromFold < 0.2) {
                        float shadeFactor = distFromFold / 0.2;
                        patternCol.rgb *= (0.7 + 0.3 * shadeFactor);
                    }
                }
            }
            else {
                // 下のページ（s_paletteから）
                vec2 texUV = uvMin + v_uv * uvSize;
                patternCol = texture2D(s_palette, texUV) * v_fillColor;
            }
        }
        
        // 折り目エッジ
        float edgeDist = abs(v_uv.x - foldX);
        if (edgeDist < 0.008) {
            patternCol.rgb *= (1.0 - (1.0 - edgeDist / 0.008) * 0.5);
        }
        
        // 見開きの綴じ目（めくった紙の上には描画しない）
        if (isSpread && !isFlippedPage) {
            float spineDist = abs(v_uv.x - 0.5);
            if (spineDist < 0.015) {
                patternCol.rgb *= (0.75 + 0.25 * spineDist / 0.015);
            }
        }
        
        // ページめくりモードでは radius=0 として扱う（角丸なし）
        // aa はそのまま使用
        radius = 0.0;
    }
    // ----------------------------------------------------------
    // RawImage (純粋テクスチャ描画、SDF/影/ボーダーなし)
    // ----------------------------------------------------------
    else if (mode < MODE_END) {
        vec2 uvMin = vec2(u_angle, curlRadius);
        vec2 uvMax = scrollXY;
        vec2 texUV = mix(uvMin, uvMax, clamp(v_uv, 0.0, 1.0));
        vec4 col = texture2D(s_palette, texUV) * v_fillColor;
        col.rgb *= col.a;
        if (col.a < 0.001) {
            discard;
        }
        gl_FragColor = col;
        return;
    }

    // ============================================================
    // per-corner radius アンパック
    // ============================================================

    if (mode < MODE_PAGE_CURL) {
        // Fill/Image/Pattern: v_scroll.zw に half16 パックされた4角radius
        uint rPack0 = floatBitsToUint(v_scroll.z);
        uint rPack1 = floatBitsToUint(v_scroll.w);
        float rTL = unpackHalfHi(rPack0);
        float rTR = unpackHalfLo(rPack0);
        float rBR = unpackHalfHi(rPack1);
        float rBL = unpackHalfLo(rPack1);
        // sdRoundBox は Y上向き座標系用なので、画面座標(Y下向き)では T↔B を入替
        // これにより C++ の radiusTL が画面左上に、radiusBL が画面左下に正しく対応
        cornerRadii = vec4(rBR, rTR, rTL, rBL);
    }
    // PageCurl: cornerRadii は vec4(0) のまま（角丸なし）

    // ============================================================
    // SDF計算
    // ============================================================

    vec2 halfSize = resolution * 0.5;
    vec2 localPos = p - halfSize;
    float maxR = min(halfSize.x, halfSize.y);

    // 各角のradiusをクランプ
    vec4 rSafe = min(max(cornerRadii, 0.0), vec4(maxR, maxR, maxR, maxR));
    float aaSafe = max(aa, 0.001);
    float shadowBlurSafe = max(shadowBlur, 0.001);

    // ============================================================
    // Corner pattern: SDF拡張 + 凹角（concave corner）
    // ============================================================
    // cornerPattern (u_param1.z):
    //   0: 通常 (全辺あり)
    //   1: 下辺なし (タブ上部)     2: 上辺なし
    //   3: 左辺なし               4: 右辺なし
    //   5: 下+右辺なし (左上角のみ) 6: 下+左辺なし (右上角のみ)
    //   7: 上+右辺なし (左下角のみ) 8: 上+左辺なし (右下角のみ)

    float cornerPattern = u_param1.z;

    // 凹角の距離 (大きい正の値 = 寄与なし)
    float concaveFill = 1000.0;
    float concaveShadow = 1000.0;
    float concaveInner = 1000.0;

    // 内側SDF拡張 (オープン辺のボーダーだけ消す)
    vec2 innerSdfExt = vec2(0.0, 0.0);
    vec2 innerSdfShift = vec2(0.0, 0.0);

    if (cornerPattern > 0.5) {
        // 凹角 (concave corners): オフセット中心による四分円SDF
        // 円の中心を矩形角から radius だけオフセットし、辺との G1 接線連続を実現
        // 例: 画面BL凹角 → 中心 (-hx - r, hy - r), 左辺と下辺に接する
        // cornerRadii 構築時に T↔B 入替済みなので:
        //   rSafe.x = rBR → 画面右下
        //   rSafe.y = rTR → 画面右上
        //   rSafe.z = rTL → 画面左上
        //   rSafe.w = rBL → 画面左下
        float scrTL = rSafe.z;  // 画面左上 = C++ radiusTL
        float scrTR = rSafe.y;  // 画面右上 = C++ radiusTR
        float scrBL = rSafe.w;  // 画面左下 = C++ radiusBL
        float scrBR = rSafe.x;  // 画面右下 = C++ radiusBR

        vec2 sLocal = localPos - shadowOffset;
        float hx = halfSize.x;
        float hy = halfSize.y;
        float bExt = borderWidth + aaSafe * 2.0;

        if (cornerPattern < 1.5) {
            // 1: 下辺なし → 画面BL,BR が凹角
            rSafe.w = 0.0;  // 画面BL凸なし
            rSafe.x = 0.0;  // 画面BR凸なし
            innerSdfExt = vec2(0.0, bExt);  innerSdfShift = vec2(0.0, bExt);
            vec2 blC = vec2(-hx - scrBL, hy - scrBL);
            vec2 brC = vec2( hx + scrBR, hy - scrBR);
            float blD = length(localPos - blC);
            float brD = length(localPos - brC);
            float blM = max(-localPos.x - hx - scrBL, hy - scrBL - localPos.y);
            float brM = max( localPos.x - hx - scrBR, hy - scrBR - localPos.y);
            concaveFill = min(max(blD - scrBL, blM), max(brD - scrBR, brM));
            float blSD = length(sLocal - blC);
            float brSD = length(sLocal - brC);
            float blSM = max(-sLocal.x - hx - scrBL, hy - scrBL - sLocal.y);
            float brSM = max( sLocal.x - hx - scrBR, hy - scrBR - sLocal.y);
            concaveShadow = min(max(blSD - scrBL, blSM), max(brSD - scrBR, brSM));
            float blIR = max(scrBL - borderWidth, 0.0);
            float brIR = max(scrBR - borderWidth, 0.0);
            concaveInner = min(max(blD - blIR, blM), max(brD - brIR, brM));
        } else if (cornerPattern < 2.5) {
            // 2: 上辺なし → 画面TL,TR が凹角
            rSafe.z = 0.0;  // 画面TL凸なし
            rSafe.y = 0.0;  // 画面TR凸なし
            innerSdfExt = vec2(0.0, bExt);  innerSdfShift = vec2(0.0, -bExt);
            vec2 tlC = vec2(-hx - scrTL, -hy + scrTL);
            vec2 trC = vec2( hx + scrTR, -hy + scrTR);
            float tlD = length(localPos - tlC);
            float trD = length(localPos - trC);
            float tlM = max(-localPos.x - hx - scrTL, localPos.y + hy - scrTL);
            float trM = max( localPos.x - hx - scrTR, localPos.y + hy - scrTR);
            concaveFill = min(max(tlD - scrTL, tlM), max(trD - scrTR, trM));
            float tlSD = length(sLocal - tlC);
            float trSD = length(sLocal - trC);
            float tlSM = max(-sLocal.x - hx - scrTL, sLocal.y + hy - scrTL);
            float trSM = max( sLocal.x - hx - scrTR, sLocal.y + hy - scrTR);
            concaveShadow = min(max(tlSD - scrTL, tlSM), max(trSD - scrTR, trSM));
            float tlIR = max(scrTL - borderWidth, 0.0);
            float trIR = max(scrTR - borderWidth, 0.0);
            concaveInner = min(max(tlD - tlIR, tlM), max(trD - trIR, trM));
        } else if (cornerPattern < 3.5) {
            // 3: 左辺なし → 画面TL,BL が凹角
            rSafe.z = 0.0;  // 画面TL凸なし
            rSafe.w = 0.0;  // 画面BL凸なし
            innerSdfExt = vec2(bExt, 0.0);  innerSdfShift = vec2(-bExt, 0.0);
            vec2 tlC = vec2(-hx - scrTL, -hy + scrTL);
            vec2 blC = vec2(-hx - scrBL,  hy - scrBL);
            float tlD = length(localPos - tlC);
            float blD = length(localPos - blC);
            float tlM = max(-localPos.x - hx - scrTL, localPos.y + hy - scrTL);
            float blM = max(-localPos.x - hx - scrBL, hy - scrBL - localPos.y);
            concaveFill = min(max(tlD - scrTL, tlM), max(blD - scrBL, blM));
            float tlSD = length(sLocal - tlC);
            float blSD = length(sLocal - blC);
            float tlSM = max(-sLocal.x - hx - scrTL, sLocal.y + hy - scrTL);
            float blSM = max(-sLocal.x - hx - scrBL, hy - scrBL - sLocal.y);
            concaveShadow = min(max(tlSD - scrTL, tlSM), max(blSD - scrBL, blSM));
            float tlIR = max(scrTL - borderWidth, 0.0);
            float blIR = max(scrBL - borderWidth, 0.0);
            concaveInner = min(max(tlD - tlIR, tlM), max(blD - blIR, blM));
        } else if (cornerPattern < 4.5) {
            // 4: 右辺なし → 画面TR,BR が凹角
            rSafe.y = 0.0;  // 画面TR凸なし
            rSafe.x = 0.0;  // 画面BR凸なし
            innerSdfExt = vec2(bExt, 0.0);  innerSdfShift = vec2(bExt, 0.0);
            vec2 trC = vec2(hx + scrTR, -hy + scrTR);
            vec2 brC = vec2(hx + scrBR,  hy - scrBR);
            float trD = length(localPos - trC);
            float brD = length(localPos - brC);
            float trM = max(localPos.x - hx - scrTR, localPos.y + hy - scrTR);
            float brM = max(localPos.x - hx - scrBR, hy - scrBR - localPos.y);
            concaveFill = min(max(trD - scrTR, trM), max(brD - scrBR, brM));
            float trSD = length(sLocal - trC);
            float brSD = length(sLocal - brC);
            float trSM = max(sLocal.x - hx - scrTR, sLocal.y + hy - scrTR);
            float brSM = max(sLocal.x - hx - scrBR, hy - scrBR - sLocal.y);
            concaveShadow = min(max(trSD - scrTR, trSM), max(brSD - scrBR, brSM));
            float trIR = max(scrTR - borderWidth, 0.0);
            float brIR = max(scrBR - borderWidth, 0.0);
            concaveInner = min(max(trD - trIR, trM), max(brD - brIR, brM));
        } else if (cornerPattern < 5.5) {
            // 5: 下+右辺なし → 画面TLのみ凸, 画面TR/BR/BL が凹角
            rSafe.y = 0.0; rSafe.x = 0.0; rSafe.w = 0.0;
            innerSdfExt = vec2(bExt, bExt);  innerSdfShift = vec2(bExt, bExt);
            vec2 trC = vec2( hx + scrTR, -hy + scrTR);
            vec2 brC = vec2( hx + scrBR,  hy - scrBR);
            vec2 blC = vec2(-hx - scrBL,  hy - scrBL);
            float trD = length(localPos - trC);
            float brD = length(localPos - brC);
            float blD = length(localPos - blC);
            float trM = max( localPos.x - hx - scrTR, localPos.y + hy - scrTR);
            float brM = max( localPos.x - hx - scrBR, hy - scrBR - localPos.y);
            float blM = max(-localPos.x - hx - scrBL, hy - scrBL - localPos.y);
            concaveFill = min(min(
                max(trD - scrTR, trM),
                max(brD - scrBR, brM)),
                max(blD - scrBL, blM));
            float trSD = length(sLocal - trC);
            float brSD = length(sLocal - brC);
            float blSD = length(sLocal - blC);
            float trSM = max( sLocal.x - hx - scrTR, sLocal.y + hy - scrTR);
            float brSM = max( sLocal.x - hx - scrBR, hy - scrBR - sLocal.y);
            float blSM = max(-sLocal.x - hx - scrBL, hy - scrBL - sLocal.y);
            concaveShadow = min(min(
                max(trSD - scrTR, trSM),
                max(brSD - scrBR, brSM)),
                max(blSD - scrBL, blSM));
            float trIR = max(scrTR - borderWidth, 0.0);
            float brIR = max(scrBR - borderWidth, 0.0);
            float blIR = max(scrBL - borderWidth, 0.0);
            concaveInner = min(min(
                max(trD - trIR, trM),
                max(brD - brIR, brM)),
                max(blD - blIR, blM));
        } else if (cornerPattern < 6.5) {
            // 6: 下+左辺なし → 画面TRのみ凸, 画面TL/BR/BL が凹角
            rSafe.z = 0.0; rSafe.x = 0.0; rSafe.w = 0.0;
            innerSdfExt = vec2(bExt, bExt);  innerSdfShift = vec2(-bExt, bExt);
            vec2 tlC = vec2(-hx - scrTL, -hy + scrTL);
            vec2 brC = vec2( hx + scrBR,  hy - scrBR);
            vec2 blC = vec2(-hx - scrBL,  hy - scrBL);
            float tlD = length(localPos - tlC);
            float brD = length(localPos - brC);
            float blD = length(localPos - blC);
            float tlM = max(-localPos.x - hx - scrTL, localPos.y + hy - scrTL);
            float brM = max( localPos.x - hx - scrBR, hy - scrBR - localPos.y);
            float blM = max(-localPos.x - hx - scrBL, hy - scrBL - localPos.y);
            concaveFill = min(min(
                max(tlD - scrTL, tlM),
                max(brD - scrBR, brM)),
                max(blD - scrBL, blM));
            float tlSD = length(sLocal - tlC);
            float brSD = length(sLocal - brC);
            float blSD = length(sLocal - blC);
            float tlSM = max(-sLocal.x - hx - scrTL, sLocal.y + hy - scrTL);
            float brSM = max( sLocal.x - hx - scrBR, hy - scrBR - sLocal.y);
            float blSM = max(-sLocal.x - hx - scrBL, hy - scrBL - sLocal.y);
            concaveShadow = min(min(
                max(tlSD - scrTL, tlSM),
                max(brSD - scrBR, brSM)),
                max(blSD - scrBL, blSM));
            float tlIR = max(scrTL - borderWidth, 0.0);
            float brIR = max(scrBR - borderWidth, 0.0);
            float blIR = max(scrBL - borderWidth, 0.0);
            concaveInner = min(min(
                max(tlD - tlIR, tlM),
                max(brD - brIR, brM)),
                max(blD - blIR, blM));
        } else if (cornerPattern < 7.5) {
            // 7: 上+右辺なし → 画面BLのみ凸, 画面TL/TR/BR が凹角
            rSafe.z = 0.0; rSafe.y = 0.0; rSafe.x = 0.0;
            innerSdfExt = vec2(bExt, bExt);  innerSdfShift = vec2(bExt, -bExt);
            vec2 tlC = vec2(-hx - scrTL, -hy + scrTL);
            vec2 trC = vec2( hx + scrTR, -hy + scrTR);
            vec2 brC = vec2( hx + scrBR,  hy - scrBR);
            float tlD = length(localPos - tlC);
            float trD = length(localPos - trC);
            float brD = length(localPos - brC);
            float tlM = max(-localPos.x - hx - scrTL, localPos.y + hy - scrTL);
            float trM = max( localPos.x - hx - scrTR, localPos.y + hy - scrTR);
            float brM = max( localPos.x - hx - scrBR, hy - scrBR - localPos.y);
            concaveFill = min(min(
                max(tlD - scrTL, tlM),
                max(trD - scrTR, trM)),
                max(brD - scrBR, brM));
            float tlSD = length(sLocal - tlC);
            float trSD = length(sLocal - trC);
            float brSD = length(sLocal - brC);
            float tlSM = max(-sLocal.x - hx - scrTL, sLocal.y + hy - scrTL);
            float trSM = max( sLocal.x - hx - scrTR, sLocal.y + hy - scrTR);
            float brSM = max( sLocal.x - hx - scrBR, hy - scrBR - sLocal.y);
            concaveShadow = min(min(
                max(tlSD - scrTL, tlSM),
                max(trSD - scrTR, trSM)),
                max(brSD - scrBR, brSM));
            float tlIR = max(scrTL - borderWidth, 0.0);
            float trIR = max(scrTR - borderWidth, 0.0);
            float brIR = max(scrBR - borderWidth, 0.0);
            concaveInner = min(min(
                max(tlD - tlIR, tlM),
                max(trD - trIR, trM)),
                max(brD - brIR, brM));
        } else {
            // 8: 上+左辺なし → 画面BRのみ凸, 画面TL/TR/BL が凹角
            rSafe.z = 0.0; rSafe.y = 0.0; rSafe.w = 0.0;
            innerSdfExt = vec2(bExt, bExt);  innerSdfShift = vec2(-bExt, -bExt);
            vec2 tlC = vec2(-hx - scrTL, -hy + scrTL);
            vec2 trC = vec2( hx + scrTR, -hy + scrTR);
            vec2 blC = vec2(-hx - scrBL,  hy - scrBL);
            float tlD = length(localPos - tlC);
            float trD = length(localPos - trC);
            float blD = length(localPos - blC);
            float tlM = max(-localPos.x - hx - scrTL, localPos.y + hy - scrTL);
            float trM = max( localPos.x - hx - scrTR, localPos.y + hy - scrTR);
            float blM = max(-localPos.x - hx - scrBL, hy - scrBL - localPos.y);
            concaveFill = min(min(
                max(tlD - scrTL, tlM),
                max(trD - scrTR, trM)),
                max(blD - scrBL, blM));
            float tlSD = length(sLocal - tlC);
            float trSD = length(sLocal - trC);
            float blSD = length(sLocal - blC);
            float tlSM = max(-sLocal.x - hx - scrTL, sLocal.y + hy - scrTL);
            float trSM = max( sLocal.x - hx - scrTR, sLocal.y + hy - scrTR);
            float blSM = max(-sLocal.x - hx - scrBL, hy - scrBL - sLocal.y);
            concaveShadow = min(min(
                max(tlSD - scrTL, tlSM),
                max(trSD - scrTR, trSM)),
                max(blSD - scrBL, blSM));
            float tlIR = max(scrTL - borderWidth, 0.0);
            float trIR = max(scrTR - borderWidth, 0.0);
            float blIR = max(scrBL - borderWidth, 0.0);
            concaveInner = min(min(
                max(tlD - tlIR, tlM),
                max(trD - trIR, trM)),
                max(blD - blIR, blM));
        }
    }

    vec2 sdfHalf = halfSize;
    vec2 sdfPos = localPos;

    // ============================================================
    // Shadow (ベース矩形 + 凹角のユニオン)
    // ============================================================

    vec2 shadowPos = sdfPos - shadowOffset;
    float shadowDist = min(sdRoundBox(shadowPos, sdfHalf, rSafe), concaveShadow);
    float shadowAlpha = 1.0 - smoothstep(-shadowBlurSafe, shadowBlurSafe, shadowDist);

    // ============================================================
    // Fill (ベース矩形 + 凹角のユニオン)
    // ============================================================

    float dist = min(sdRoundBox(sdfPos, sdfHalf, rSafe), concaveFill);
    float alphaFill = smoothstep(aaSafe, -aaSafe, dist);

    // ============================================================
    // Border (ベース矩形 + 凹角のユニオン, 内側も同様)
    // ============================================================

    vec4 innerR = max(rSafe - vec4(borderWidth, borderWidth, borderWidth, borderWidth), 0.0);
    vec2 innerHalf = sdfHalf + innerSdfExt - vec2(borderWidth, borderWidth);
    vec2 innerPos = sdfPos - innerSdfShift;
    float distInner = min(
        sdRoundBox(innerPos, innerHalf, innerR),
        concaveInner);
    float alphaInner = smoothstep(aaSafe, -aaSafe, distInner);
    float alphaBorder = clamp(alphaFill - alphaInner, 0.0, 1.0);
    
    // ============================================================
    // 合成
    // ============================================================

    if (blendMode < 0.5) {
        // --- プリマルチプライドモード (背景と合成) ---
        // Shadow (premultiply)
        vec4 shadow = v_shadowColor;
        shadow.a *= shadowAlpha;
        shadow.rgb *= shadow.a;

        // Pattern (内側のみ, premultiply)
        vec4 pattern = patternCol;
        pattern.rgb *= pattern.a;
        pattern *= alphaInner;

        // Border色 (premultiply)
        vec4 bord = v_borderColor;
        bord.rgb *= bord.a;
        bord *= alphaBorder;

        vec4 col = shadow;

        // Pattern over Shadow
        col.rgb = col.rgb * (1.0 - pattern.a) + pattern.rgb;
        col.a = col.a + pattern.a * (1.0 - col.a);

        // Border over result
        col.rgb = col.rgb * (1.0 - bord.a) + bord.rgb;
        col.a = col.a + bord.a * (1.0 - col.a);

        if (col.a < 0.001) {
            discard;
        }
        gl_FragColor = col;
    }
    else {
        // --- 上書きモード (背景を反映しない) ---
        // 優先度: ボーダー > パネル > 影 (分岐なし)
        float bordA  = v_borderColor.a * alphaBorder;
        float panelA = patternCol.a * alphaInner;
        float shadA  = v_shadowColor.a * shadowAlpha * (1.0 - alphaFill);

        // ボーダーがあればボーダー、なければパネル
        float useBord = step(0.001, bordA);
        vec3 rgb = mix(patternCol.rgb, v_borderColor.rgb, useBord);
        float a  = mix(panelA, bordA, useBord);

        // パネル/ボーダーがなければ影
        float useFill = step(0.001, a);
        rgb = mix(v_shadowColor.rgb, rgb, useFill);
        a   = mix(shadA, a, useFill);

        if (a < 0.001) {
            discard;
        }
        gl_FragColor = vec4(rgb * a, a);
    }
}