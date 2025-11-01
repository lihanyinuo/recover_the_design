# Comprehensive Test Report Generator
$testCases = @(
    "test_empty",
    "test_single", 
    "test_simple",
    "test_chain",
    "test_dense",
    "test_degree4",
    "test_corner",
    "test_conflict",
    "test1",
    "test2",
    "test3",
    "test_large",
    "test_worst_case",
    "test_huge"
)

$results = @()

Write-Host "Starting comprehensive testing..." -ForegroundColor Cyan
Write-Host ""

foreach ($test in $testCases) {
    $file = "$test.txt"
    
    if (-not (Test-Path $file)) {
        Write-Host "[$test] File not found - SKIP" -ForegroundColor Red
        continue
    }
    
    Write-Host "Testing: $test" -ForegroundColor Yellow
    
    # Test original version
    $output1 = Get-Content $file | ..\src\recover_design.exe 2>&1
    $time1 = Measure-Command { Get-Content $file | ..\src\recover_design.exe | Out-Null }
    
    # Test optimized version  
    $output2 = Get-Content $file | ..\src\recover_design_optimized.exe 2>&1
    $time2 = Measure-Command { Get-Content $file | ..\src\recover_design_optimized.exe | Out-Null }
    
    # Analyze results
    $result1 = if ($output1 -match "No Solution") { "No Solution" } else { "$($output1.Count) lines" }
    $result2 = if ($output2 -match "No Solution") { "No Solution" } else { "$($output2.Count) lines" }
    $match = if ($output1 -join "`n" -eq $output2 -join "`n") { "PASS" } else { "FAIL" }
    
    $results += [PSCustomObject]@{
        TestCase = $test
        Result_Original = $result1
        Result_Optimized = $result2
        Correctness = $match
        Time_Original_ms = [math]::Round($time1.TotalMilliseconds, 2)
        Time_Optimized_ms = [math]::Round($time2.TotalMilliseconds, 2)
        Ticks_Original = $time1.Ticks
        Ticks_Optimized = $time2.Ticks
        Speedup = if ($time2.TotalMilliseconds -gt 0) { [math]::Round($time1.TotalMilliseconds / $time2.TotalMilliseconds, 2) } else { "N/A" }
    }
    
    Write-Host "  Original: $result1 ($($time1.TotalMilliseconds) ms)" -ForegroundColor Gray
    Write-Host "  Optimized: $result2 ($($time2.TotalMilliseconds) ms)" -ForegroundColor Gray
    Write-Host "  Status: $match" -ForegroundColor $(if ($match -eq "PASS") { "Green" } else { "Red" })
    Write-Host ""
}

# Output results table
Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "TEST RESULTS SUMMARY" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

$results | Format-Table -AutoSize

# Save to CSV
$results | Export-Csv -Path "test_results.csv" -NoTypeInformation -Encoding UTF8
Write-Host "Results saved to test_results.csv" -ForegroundColor Green

# Statistics
$passCount = ($results | Where-Object { $_.Correctness -eq "PASS" }).Count
$totalTests = $results.Count
$totalTimeOriginal = ($results | Measure-Object -Property Time_Original_ms -Sum).Sum
$totalTimeOptimized = ($results | Measure-Object -Property Time_Optimized_ms -Sum).Sum
$overallSpeedup = if ($totalTimeOptimized -gt 0) { [math]::Round($totalTimeOriginal / $totalTimeOptimized, 2) } else { "N/A" }

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "STATISTICS" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Total Tests: $totalTests"
Write-Host "Passed: $passCount" -ForegroundColor Green
Write-Host "Failed: $($totalTests - $passCount)" -ForegroundColor $(if ($totalTests -eq $passCount) { "Green" } else { "Red" })
Write-Host "Pass Rate: $([math]::Round(100.0 * $passCount / $totalTests, 1))%"
Write-Host ""
Write-Host "Total Time (Original): $([math]::Round($totalTimeOriginal, 2)) ms" -ForegroundColor Cyan
Write-Host "Total Time (Optimized): $([math]::Round($totalTimeOptimized, 2)) ms" -ForegroundColor Cyan
Write-Host "Overall Speedup: ${overallSpeedup}x" -ForegroundColor $(if ($overallSpeedup -gt 1) { "Green" } elseif ($overallSpeedup -lt 1) { "Red" } else { "Gray" })
Write-Host "========================================`n" -ForegroundColor Cyan
