# 批量生成随机测试用例
# 使用方法: .\generate_random_tests.ps1

Write-Host "=== 随机测试用例生成器 ===" -ForegroundColor Cyan
Write-Host ""

# 编译生成器
Write-Host "正在编译测试用例生成器..." -ForegroundColor Yellow
Set-Location ..\src
gcc generate_testcase.c -o generate_testcase.exe

if ($LASTEXITCODE -ne 0) {
    Write-Host "编译失败！" -ForegroundColor Red
    Set-Location ..\test
    exit 1
}

Write-Host "编译成功！" -ForegroundColor Green
Write-Host ""

# 返回 test 目录
Set-Location ..\test

# 定义测试用例配置
$testCases = @(
    @{Name="random_tiny"; Rows=3; Cols=3; Density=0.5; Seed=1001},
    @{Name="random_small_sparse"; Rows=5; Cols=5; Density=0.3; Seed=2001},
    @{Name="random_small_dense"; Rows=5; Cols=5; Density=0.7; Seed=2002},
    @{Name="random_medium"; Rows=10; Cols=10; Density=0.5; Seed=3001},
    @{Name="random_medium_sparse"; Rows=10; Cols=10; Density=0.3; Seed=3002},
    @{Name="random_medium_dense"; Rows=10; Cols=10; Density=0.7; Seed=3003},
    @{Name="random_large"; Rows=20; Cols=20; Density=0.5; Seed=4001},
    @{Name="random_large_sparse"; Rows=20; Cols=20; Density=0.3; Seed=4002},
    @{Name="random_large_dense"; Rows=15; Cols=15; Density=0.7; Seed=4003},
    @{Name="random_rect_wide"; Rows=5; Cols=15; Density=0.5; Seed=5001},
    @{Name="random_rect_tall"; Rows=15; Cols=5; Density=0.5; Seed=5002}
)

Write-Host "开始生成测试用例..." -ForegroundColor Cyan
Write-Host ""

foreach ($test in $testCases) {
    $fileName = "$($test.Name).txt"
    Write-Host "生成: $fileName ($($test.Rows)x$($test.Cols), 密度=$($test.Density))" -ForegroundColor Yellow
    
    ..\src\generate_testcase.exe $test.Rows $test.Cols $test.Density $test.Seed > $fileName 2>&1
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "  ✓ 生成成功" -ForegroundColor Green
    } else {
        Write-Host "  ✗ 生成失败" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "=== 生成完成 ===" -ForegroundColor Cyan
Write-Host ""
Write-Host "生成的测试文件列表:" -ForegroundColor Yellow
Get-ChildItem -Filter "random_*.txt" | ForEach-Object {
    $size = [math]::Round($_.Length / 1KB, 2)
    Write-Host "  $($_.Name) ($size KB)"
}

Write-Host ""
Write-Host "提示: 使用以下命令测试生成的用例" -ForegroundColor Cyan
Write-Host "  ..\src\recover_design.exe < random_small_sparse.txt" -ForegroundColor Gray
Write-Host "  ..\src\recover_design_optimized.exe < random_medium.txt" -ForegroundColor Gray
