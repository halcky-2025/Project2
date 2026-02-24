$jsonlPath = "C:\Users\hayah\.claude\projects\C--Users-hayah-source-repos-Project2-Project2\2091347a-33c5-40ea-badc-b18db40789a2.jsonl"
$outPath = "C:\Users\hayah\source\repos\Project2\Project2\temp_cs3.txt"

$line = Get-Content $jsonlPath -TotalCount 36 | Select-Object -Last 1

# Find the start of the third C# block
$idx = $line.IndexOf("partial class Word")
if ($idx -lt 0) {
    Write-Host "Pattern not found"
    exit 1
}

# Go back a bit to find the block start
$startIdx = [Math]::Max(0, $idx - 200)
$searchBack = $line.Substring($startIdx, $idx - $startIdx)
Write-Host "Context before: $searchBack"

# Find end of the block - search for closing pattern
# The C# code should end with a closing brace pattern
$sub = $line.Substring($idx)

# Find where the C# code ends - look for a JSON boundary like ","type": or similar
$endPatterns = @('","type":', '"}],"type":', '\\n}\\n', '"\n}')
$endIdx = $sub.Length
foreach ($pat in $endPatterns) {
    $found = $sub.IndexOf($pat)
    if ($found -gt 0 -and $found -lt $endIdx) {
        $endIdx = $found
    }
}

$csCode = $sub.Substring(0, $endIdx)

# Unescape JSON string
$csCode = $csCode.Replace('\n', "`n")
$csCode = $csCode.Replace('\t', "`t")
$csCode = $csCode.Replace('\"', '"')
$csCode = $csCode.Replace('\\', '\')

Set-Content -Path $outPath -Value $csCode -Encoding UTF8
$lineCount = ($csCode -split "`n").Count
Write-Host "Extracted $lineCount lines to $outPath"
