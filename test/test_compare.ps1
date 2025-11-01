# 对比测试脚本 - 原版 vs 优化版
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   原版 vs 优化版 对比测试" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$tests = @(
    @{name="test_empty"; desc="空网格（全0）"},
    @{name="test_single"; desc="单个孤立连接器"},
    @{name="test_simple"; desc="简单2x2有解"},
    @{name="test_chain"; desc="链式结构"},
    @{name="test_dense"; desc="密集连接器"},
    @{name="test_degree4"; desc="度数4测试"},
    @{name="test_corner"; desc="四角连接器"},
    @{name="test_conflict"; desc="约束冲突"},
    @{name="test1"; desc="标准测试1"},
    @{name="test2"; desc="标准测试2"},
    @{name="test3"; desc="标准测试3"},
    @{name="test_large"; desc="大规模20x20"},
    @{name="test_worst_case"; desc="最坏情况6x6"},
    @{name="test_huge"; desc="超大规模30x30"}
)

$totalOriginal = 0
$totalOptimized = 0
$passCount = 0
$failCount = 0

foreach ($test in $tests) {
    Write-Host "[$($test.name)]" -ForegroundColor Yellow -NoNewline
    Write-Host " - $($test.desc)" -ForegroundColor Gray
    
    if (Test-Path "$($test.name).txt") {
        # 测试原版
        $startTime = Get-Date
        $output1 = Get-Content "$($test.name).txt" | .\recover_design.exe 2>&1
        $endTime = Get-Date
        $time1 = ($endTime - $startTime).TotalMilliseconds
        $totalOriginal += $time1
        
        # 测试优化版
        $startTime = Get-Date
        $output2 = Get-Content "$($test.name).txt" | .\recover_design_optimized.exe 2>&1
        $endTime = Get-Date
        $time2 = ($endTime - $startTime).TotalMilliseconds
        $totalOptimized += $time2
        
        # 比较结果
        $result1 = if ($output1 -match "No Solution") { "No Solution" } else { ($output1 | Measure-Object -Line).Lines.ToString() + " 行" }
        $result2 = if ($output2 -match "No Solution") { "No Solution" } else { ($output2 | Measure-Object -Line).Lines.ToString() + " 行" }
        
        if ($output1 -eq $output2) {
            Write-Host "  ✓ 结果一致: " -ForegroundColor Green -NoNewline
            Write-Host "$result1" -ForegroundColor White
            $passCount++
        } else {
            Write-Host "  ✗ 结果不一致!" -ForegroundColor Red
            Write-Host "    原版: $result1" -ForegroundColor Yellow
            Write-Host "    优化: $result2" -ForegroundColor Yellow
            $failCount++
        }
        
        $speedup = if ($time2 -gt 0) { [math]::Round($time1 / $time2, 2) } else { "∞" }
        Write-Host "  原版耗时: " -NoNewline
        Write-Host "$([math]::Round($time1, 2)) ms" -ForegroundColor Cyan
        Write-Host "  优化耗时: " -NoNewline
        Write-Host "$([math]::Round($time2, 2)) ms" -ForegroundColor Cyan
        Write-Host "  加速比: " -NoNewline
        if ($speedup -is [double] -and $speedup -gt 1) {
            Write-Host "${speedup}x" -ForegroundColor Green
        } elseif ($speedup -is [double] -and $speedup -lt 1) {
            Write-Host "${speedup}x" -ForegroundColor Red
        } else {
            Write-Host "$speedup" -ForegroundColor Gray
        }
    } else {
        Write-Host "  [文件不存在]" -ForegroundColor Red
    }
    Write-Host ""
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "测试总结" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "通过: " -NoNewline -ForegroundColor Green
Write-Host "$passCount"
Write-Host "失败: " -NoNewline -ForegroundColor Red
Write-Host "$failCount"
Write-Host "原版总耗时: " -NoNewline
Write-Host "$([math]::Round($totalOriginal, 2)) ms" -ForegroundColor Cyan
Write-Host "优化版总耗时: " -NoNewline
Write-Host "$([math]::Round($totalOptimized, 2)) ms" -ForegroundColor Cyan
$overallSpeedup = if ($totalOptimized -gt 0) { [math]::Round($totalOriginal / $totalOptimized, 2) } else { "∞" }
Write-Host "总体加速比: " -NoNewline
Write-Host "${overallSpeedup}x" -ForegroundColor $(if ($overallSpeedup -gt 1) { "Green" } else { "Red" })
Write-Host "========================================" -ForegroundColor Cyan
