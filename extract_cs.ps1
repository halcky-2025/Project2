$jsonPath = "C:\Users\hayah\.claude\projects\C--Users-hayah-source-repos-Project2-Project2\2091347a-33c5-40ea-badc-b18db40789a2\tool-results\toolu_01Jj6nkHahHFn5ceq7bUxvR6.json"
$raw = Get-Content -Raw $jsonPath
$data = $raw | ConvertFrom-Json
$text = $data[0].text
$idx = $text.IndexOf("partial class Word")
$code = $text.Substring($idx)
Set-Content -Path "C:\Users\hayah\source\repos\Project2\Project2\temp_cs3.txt" -Value $code -Encoding UTF8
Write-Host "Done"
