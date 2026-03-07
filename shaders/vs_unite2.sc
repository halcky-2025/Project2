$input a_position, i_data0, i_data1, i_data2, i_data3, i_data4
$output v_uv, v_rectParams, v_patternParams, v_scroll, v_shadowBorder, v_uvRange, v_fillColor, v_borderColor, v_shadowColor

#include <bgfx_shader.sh>

// ============================================================
// Varying出力の内容
// ============================================================
// v_uv:           UV座標 (0-1、影拡張時は負もあり)
// v_rectParams:   x, y, width, height
// v_patternParams:
//   .xy:           (未使用)
//   .z:            angle / uvMin.x / curlAngle
//   .w:            (未使用) / uvMin.y / curlRadius
// v_scroll:
//   Fill/Image/Pattern: scrollX, scrollY, pack(rTL,rTR)(half16×2), pack(rBR,rBL)(half16×2)
//   PageCurl: uvSize.x, uvSize.y, progress, aa
// v_shadowBorder: pack(bTop,bRight)(half16×2), pack(bBottom,bLeft)(half16×2), pack(aa,shadowBlur)(half16×2), pack(shadowX,shadowY)(half16×2)
// v_uvRange:
//   .xy:           colorCount, dataOffset (パターン、整数)
//                  uvMin.x, uvMin.y (PageCurl、整数、FSで/65535.0)
//   .zw:           backUVMin.x, backUVMin.y (ページめくりのみ、整数、FSで/65535.0)
//   ※ Imageモードでは v_uvRange 未使用
// v_fillColor:    塗りつぶし色 (RGBA, 0-1)
// v_borderColor:  ボーダー色 (RGBA, 0-1)
// v_shadowColor:  影色 (RGBA, 0-1)
// ============================================================

// half16 アンパック: uint の上位16bitまたは下位16bitから float に変換
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

uniform vec4 u_param1;  // x = mode

void main()
{
    vec4 rectParams = i_data0;      // x, y, width, height
    vec4 patternParams = i_data1;   // colorCount_dataOffset/frontUVMin, (unused)/backUVMin, angle/curlAngle, (unused)/curlRadius
    vec4 scroll = i_data2;          // scrollX/uvSize.x, scrollY/uvSize.y, pack(rTL,rTR)/progress, pack(rBR,rBL)/aa
    vec4 shadowBorder = i_data3;    // pack(bTop,bRight), pack(bBottom,bLeft), pack(aa,shadowBlur), pack(shadowX,shadowY)
    vec4 colors = i_data4;          // shadowColor(packed), fillColor(packed), borderColor(packed), zIndex/1000(float)

    // shadowX, shadowY を half16 からアンパック
    uint sxyPacked = floatBitsToUint(shadowBorder.w);
    float shadowX = unpackHalfHi(sxyPacked);
    float shadowY = unpackHalfLo(sxyPacked);

    // shadowBlur を half16 からアンパック（影マージン計算に必要）
    // pack順: aa(上位), shadowBlur(下位)
    uint sbPacked = floatBitsToUint(shadowBorder.z);
    float shadowBlur = unpackHalfLo(sbPacked);

    // 影パラメータ
    vec2 shadowOffset = vec2(shadowX, shadowY);

    // 影が必要とする追加マージン（各方向）
    float marginLeft   = max(0.0, -shadowOffset.x + shadowBlur);
    float marginRight  = max(0.0,  shadowOffset.x + shadowBlur);
    float marginTop    = max(0.0, -shadowOffset.y + shadowBlur);
    float marginBottom = max(0.0,  shadowOffset.y + shadowBlur);

    // Corner pattern: 凹角がはみ出す分のマージン拡張
    float cpMode = u_param1.z;
    if (cpMode > 0.5) {
        // per-corner radius をアンパックして最大値を取得
        uint rPack0 = floatBitsToUint(scroll.z);
        uint rPack1 = floatBitsToUint(scroll.w);
        float maxCR = max(
            max(unpackHalfHi(rPack0), unpackHalfLo(rPack0)),
            max(unpackHalfHi(rPack1), unpackHalfLo(rPack1))
        );
        marginLeft   += maxCR;
        marginRight  += maxCR;
        marginTop    += maxCR;
        marginBottom += maxCR;
    }

    // 拡張後の四角形サイズ
    float extWidth  = rectParams.z + marginLeft + marginRight;
    float extHeight = rectParams.w + marginTop + marginBottom;

    // 拡張後の位置（左上基準）
    float extX = rectParams.x - marginLeft;
    float extY = rectParams.y - marginTop;

    // 頂点位置（a_position: 0-1）を拡張四角形に適用
    vec2 pos = vec2(extX, extY) + a_position * vec2(extWidth, extHeight);

    // UV: 拡張領域を考慮（元の四角形基準で -margin ～ 1+margin）
    v_uv = (a_position * vec2(extWidth, extHeight) - vec2(marginLeft, marginTop))
           / rectParams.zw;

    // ----------------------------------------------------------
    // i_data1.x unpack (16bit×2)
    // 通常モード: colorCount, dataOffset → 整数のまま
    // ページめくり: frontUVMin.x, frontUVMin.y → 整数のまま（FSで/65535.0）
    // ----------------------------------------------------------
    uint packed0 = floatBitsToUint(patternParams.x);
    float uvRange0 = float((packed0 >> 16u) & 0xFFFFu);  // colorCount / frontUVMin.x (int)
    float uvRange1 = float(packed0 & 0xFFFFu);           // dataOffset / frontUVMin.y (int)

    // ----------------------------------------------------------
    // i_data1.y unpack (16bit×2, ページめくりモードのみ使用)
    // backUVMin.x, backUVMin.y → 整数のまま（FSで/65535.0）
    // ----------------------------------------------------------
    uint packed1 = floatBitsToUint(patternParams.y);
    float uvRange2 = float((packed1 >> 16u) & 0xFFFFu);  // backUVMin.x (int)
    float uvRange3 = float(packed1 & 0xFFFFu);           // backUVMin.y (int)

    // ----------------------------------------------------------
    // varying出力
    // ----------------------------------------------------------
    v_rectParams = rectParams;
    v_patternParams = vec4(0.0, 0.0, patternParams.z, patternParams.w);  // .xy未使用, .z=angle/curlAngle, .w=curlRadius
    v_scroll = scroll;           // FSでモード別にアンパック
    v_shadowBorder = shadowBorder; // FS側で全half16アンパック (border4辺, aa, shadowBlur, shadowXY)
    v_uvRange = vec4(uvRange0, uvRange1, uvRange2, uvRange3);

    // shadowColor アンパック (8bit×4, i_data4.x)
    uint shadowPacked = floatBitsToUint(colors.x);
    v_shadowColor = vec4(
        float((shadowPacked >> 24u) & 0xFFu) / 255.0,
        float((shadowPacked >> 16u) & 0xFFu) / 255.0,
        float((shadowPacked >>  8u) & 0xFFu) / 255.0,
        float( shadowPacked         & 0xFFu) / 255.0
    );

    // fillColor アンパック (8bit×4, i_data4.y)
    uint fillPacked = floatBitsToUint(colors.y);
    v_fillColor = vec4(
        float((fillPacked >> 24u) & 0xFFu) / 255.0,
        float((fillPacked >> 16u) & 0xFFu) / 255.0,
        float((fillPacked >>  8u) & 0xFFu) / 255.0,
        float( fillPacked         & 0xFFu) / 255.0
    );

    // borderColor アンパック (8bit×4, i_data4.z)
    uint borderPacked = floatBitsToUint(colors.z);
    v_borderColor = vec4(
        float((borderPacked >> 24u) & 0xFFu) / 255.0,
        float((borderPacked >> 16u) & 0xFFu) / 255.0,
        float((borderPacked >>  8u) & 0xFFu) / 255.0,
        float( borderPacked         & 0xFFu) / 255.0
    );

    // 深度（zIndex, i_data4.w, 直接float）
    float z = colors.w;

    gl_Position = mul(u_modelViewProj, vec4(pos, z, 1.0));
}
