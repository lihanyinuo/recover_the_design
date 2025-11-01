# 批量测试脚本 - 原版
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "        原版程序测试" -ForegroundColor Cyan
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

foreach ($test in $tests) {
    Write-Host "[$($test.name)]" -ForegroundColor Yellow -NoNewline
    Write-Host " - $($test.desc)" -ForegroundColor Gray
    
    if (Test-Path "$($test.name).txt") {
        $startTime = Get-Date
        $output = Get-Content "$($test.name).txt" | ..\src\recover_design.exe 2>&1
        $endTime = Get-Date
        $elapsed = ($endTime - $startTime).TotalMilliseconds
        
        $lineCount = ($output | Measure-Object -Line).Lines
        
        if ($output -match "No Solution") {
            Write-Host "  结果: " -NoNewline
            Write-Host "No Solution" -ForegroundColor Red
        } else {
            Write-Host "  结果: " -NoNewline
            Write-Host "$lineCount 行解" -ForegroundColor Green
        }
        Write-Host "  耗时: $([math]::Round($elapsed, 2)) ms" -ForegroundColor Cyan
    } else {
        Write-Host "  [文件不存在]" -ForegroundColor Red
    }
    Write-Host ""
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "测试完成" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
