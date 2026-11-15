适用于《星际争霸 II》国服客户端的启动时增强补丁。
1. **恢复完整画面表现**：取消国服强制启用的「减少暴力表现」，恢复血液与燃烧等死亡效果。
2. **支持切换其他语言**：可选用其他语言的语音与文字。

> **免责声明**：修改客户端违反软件许可协议，存在账号封禁风险。仅限 PvE 模式（战役、合作、人机对战）研究使用，相关风险自负。

## 上游与技术说明
画面表现部分的思路及三处 CHN 条件跳转地址源自 [shui-ben/SC2Uncensor](https://github.com/shui-ben/SC2Uncensor) (GPLv3)。本项目未引用上游代码，改为随游戏启动自动加载的 DLL，替代外部内存读写与 OCR 方案，并新增了语言切换支持。

## 部署流程
1. 终止游戏及相关进程。
2. 打开游戏文件夹。
3. 将 `pdh.dll` 复制至 `Versions\Base98370\`（与 `SC2_x64.exe` 同级），无需额外配置。
4. 启动游戏，取消勾选「减少暴力表现」，在语言和地区里面更改语言。

### Mac（CrossOver）
CrossOver 默认使用自带的 `pdh.dll`，放进游戏目录也不会被加载，需要额外设置一次：
1. 按上面的步骤放好 `pdh.dll`。
2. 打开「终端」，执行下面这条命令（把 `<容器名>` 换成装星际的容器名称）：

   ```sh
   "/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine" --bottle "<容器名>" reg add 'HKCU\Software\Wine\AppDefaults\SC2_x64.exe\DllOverrides' /v pdh /t REG_SZ /d native,builtin /f
   ```

   这条设置只对星际生效，不影响容器里的其他程序。使用 CrossOver Preview 的，把路径里的 `CrossOver.app` 换成 `CrossOver Preview.app`。
3. 启动游戏，同上。

其他 Wine 环境用 `wine reg add` 加同样的设置即可。

## 卸载
先将语言改回中文，然后删除 `Versions\Base98370\pdh.dll`。

## 版本更新维护
游戏客户端版本更新后，执行目录将变更至新版本号（如 `BaseXXXXX`）。需重新部署 DLL 并提取对应版本的 RVA 偏移及特征码。

## 许可证
GPLv3
