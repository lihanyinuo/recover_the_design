# 自定义测试用例生成器（交互式）
# 使用方法: .\generate_custom.ps1

Write-Host "=== 自定义测试用例生成器 ===" -ForegroundColor Cyan
Write-Host ""

# 检查生成器是否已编译
if (-not (Test-Path "..\src\generate_testcase.exe")) {
    Write-Host "正在编译测试用例生成器..." -ForegroundColor Yellow
    Set-Location ..\src
    gcc generate_testcase.c -o generate_testcase.exe
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "编译失败！" -ForegroundColor Red
        Set-Location ..\test
        exit 1
    }
    
    Write-Host "编译成功！" -ForegroundColor Green
    Set-Location ..\test
}

Write-Host ""

# 交互式输入
Write-Host "请输入测试用例参数:" -ForegroundColor Yellow
Write-Host ""

# 输入行数
do {
    $rows = Read-Host "行数 (1-100)"
    $rowsInt = [int]$rows
} while ($rowsInt -lt 1 -or $rowsInt -gt 100)

# 输入列数
do {
    $cols = Read-Host "列数 (1-100)"
    $colsInt = [int]$cols
} while ($colsInt -lt 1 -or $colsInt -gt 100)

# 输入密度
do {
    $density = Read-Host "连接器密度 (0.0-1.0, 推荐 0.3-0.7)"
    $densityFloat = [double]$density
} while ($densityFloat -lt 0.0 -or $densityFloat -gt 1.0)

# 输入文件名
$defaultName = "test_custom_${rowsInt}x${colsInt}.txt"
$fileName = Read-Host "输出文件名 (默认: $defaultName)"
if ([string]::IsNullOrWhiteSpace($fileName)) {
    $fileName = $defaultName
}

# 是否使用固定种子
$useSeed = Read-Host "使用固定随机种子？(y/n, 默认 n)"
if ($useSeed -eq "y" -or $useSeed -eq "Y") {
    $seed = Read-Host "随机种子 (整数)"
    $seedArg = $seed
} else {
    $seedArg = ""
}

Write-Host ""
Write-Host "正在生成测试用例..." -ForegroundColor Yellow
Write-Host "  网格大小: ${rowsInt}x${colsInt}" -ForegroundColor Gray
Write-Host "  密度: $densityFloat" -ForegroundColor Gray
Write-Host "  输出文件: $fileName" -ForegroundColor Gray
if ($seedArg -ne "") {
    Write-Host "  随机种子: $seedArg" -ForegroundColor Gray
}
Write-Host ""

# 生成测试用例
if ($seedArg -ne "") {
    ..\src\generate_testcase.exe $rowsInt $colsInt $densityFloat $seedArg > $fileName 2>&1
} else {
    ..\src\generate_testcase.exe $rowsInt $colsInt $densityFloat > $fileName 2>&1
}

if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ 生成成功！" -ForegroundColor Green
    Write-Host ""
    
    # 显示文件信息
    $fileInfo = Get-Item $fileName
    $size = [math]::Round($fileInfo.Length / 1KB, 2)
    Write-Host "文件信息:" -ForegroundColor Cyan
    Write-Host "  路径: $($fileInfo.FullName)" -ForegroundColor Gray
    Write-Host "  大小: $size KB" -ForegroundColor Gray
    
    # 预览前几行
    Write-Host ""
    Write-Host "文件预览 (前 10 行):" -ForegroundColor Cyan
    Get-Content $fileName -Head 10 | ForEach-Object {
        Write-Host "  $_" -ForegroundColor Gray
    }
    
    # 询问是否立即测试
    Write-Host ""
    $runTest = Read-Host "立即运行测试？(1=原始版本, 2=优化版本, n=不测试)"
    
    if ($runTest -eq "1") {
        Write-Host ""
        Write-Host "运行原始版本..." -ForegroundColor Yellow
        Get-Content $fileName | ..\src\recover_design.exe
    } elseif ($runTest -eq "2") {
        Write-Host ""
        Write-Host "运行优化版本..." -ForegroundColor Yellow
        Get-Content $fileName | ..\src\recover_design_optimized.exe
    }
    
} else {
    Write-Host "✗ 生成失败！" -ForegroundColor Red
}

Write-Host ""
Write-Host "完成！" -ForegroundColor Cyan
