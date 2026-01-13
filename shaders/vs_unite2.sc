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
//   パターンモード: scrollX, scrollY, radius, aa
//   Imageモード:    uvMax.x, uvMax.y, radius, aa
//   ページめくり:   uvSize.x, uvSize.y, progress, aa
// v_shadowBorder: shadowX, shadowY, shadowBlur, borderWidth
// v_uvRange:
//   .xy:           colorCount, dataOffset (パターン、整数)
//                  uvMin.x, uvMin.y (PageCurl、整数、FSで/65535.0)
//   .zw:           backUVMin.x, backUVMin.y (ページめくりのみ、整数、FSで/65535.0)
//   ※ Imageモードでは v_uvRange 未使用
// v_fillColor:    塗りつぶし色 (RGBA, 0-1)
// v_borderColor:  ボーダー色 (RGBA, 0-1)
// v_shadowColor:  影色 (RGBA, 0-1)
// ============================================================

uniform vec4 u_param1;  // x = mode

void main()
{
    vec4 rectParams = i_data0;      // x, y, width, height
    vec4 patternParams = i_data1;   // colorCount_dataOffset/frontUVMin, (unused)/backUVMin, angle/curlAngle, (unused)/curlRadius
    vec4 scroll = i_data2;          // scrollX/uvSize.x, scrollY/uvSize.y, radius/progress, aa
    vec4 shadowBorder = i_data3;    // shadowX, shadowY, shadowBlur, borderWidth
    vec4 colors = i_data4;          // shadowColor(packed), fillColor(packed), borderColor(packed), zIndex
    
    // 影パラメータ
    vec2 shadowOffset = shadowBorder.xy;
    float shadowBlur = shadowBorder.z;
    
    // 影が必要とする追加マージン（各方向）
    float marginLeft   = max(0.0, -shadowOffset.x + shadowBlur);
    float marginRight  = max(0.0,  shadowOffset.x + shadowBlur);
    float marginTop    = max(0.0, -shadowOffset.y + shadowBlur);
    float marginBottom = max(0.0,  shadowOffset.y + shadowBlur);
    
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
    v_scroll = scroll;
    v_shadowBorder = shadowBorder;
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
    
    // 深度（zIndex, i_data4.w）
    float z = colors.w / 65535.0;
    
    gl_Position = mul(u_modelViewProj, vec4(pos, z, 1.0));
}