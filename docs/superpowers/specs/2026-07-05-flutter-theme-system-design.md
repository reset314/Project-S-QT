# Flutter 主题系统 — 设计文档

**日期:** 2026-07-05
**范围:** aaa1 Flutter 客户端主题系统迁移，对齐 aaa2 Qt CSS 主题规范

## 1. 概述

将 [Project-S UI Redesign](./2026-07-04-project-s-ui-redesign.md) 中定义的 CSS 主题系统迁移到 Flutter。核心目标：

- 与 aaa2 共享 CSS 主题文件格式（`:root` 变量块）
- 独立 `DesignTokens` 数据类 + Riverpod provider 驱动
- 3 套内置主题：light / dark / glass
- 玻璃主题使用 BackdropFilter 实时模糊
- 支持自定义 CSS 导入

## 2. CSS 令牌规范

与 aaa2 共用同一套 CSS 变量定义。变量名、值格式完全一致。

### 2.1 颜色 (22 个)

| 变量 | 用途 |
|------|------|
| `--primary`, `--primary-light`, `--primary-dark` | 主色调 |
| `--accent`, `--accent-light`, `--accent-dark` | 强调色 |
| `--bg-main`, `--bg-surface`, `--bg-sidebar`, `--bg-titlebar`, `--bg-input` | 背景色 |
| `--divider` | 分割线 |
| `--text-primary`, `--text-secondary`, `--text-hint` | 文字色 |
| `--text-on-primary`, `--text-on-accent` | 主色/强调色上的文字 |
| `--user-bubble`, `--user-bubble-text` | 用户气泡 |
| `--ai-bubble`, `--ai-bubble-text` | AI 气泡 |
| `--system-bubble`, `--system-bubble-text` | 系统气泡 |
| `--sidebar-hover`, `--sidebar-selected` | 侧边栏状态 |
| `--badge`, `--badge-text` | 角标 |
| `--error`, `--warning`, `--success` | 语义色 |
| `--online`, `--offline` | 在线状态 |

### 2.2 字体 (8 个)

| 变量 | 默认值 |
|------|--------|
| `--font-family` | `""` |
| `--font-scale` | `1.0` |
| `--font-caption` ~ `--font-display` | `11px` ~ `24px` |

### 2.3 间距 (5 个)

`--spacing-tiny: 4px` ~ `--spacing-xl: 24px`

### 2.4 圆角 (3 个)

`--bubble-radius`, `--card-radius`, `--button-radius`

### 2.5 玻璃效果 (2 个)

`--glass-blur` (px), `--glass-opacity` (0~1)

### 2.6 动画 (3 个)

`--anim-fast`, `--anim-normal`, `--anim-slow` (ms)

### 2.7 布局 (6 个)

`--avatar-sm/md/lg/xl`, `--icon-sm/md/lg` (px)

## 3. 架构

```
lib/core/theme/
├── design_tokens.dart        # 不可变数据类，字段一一对应 CSS 变量
├── theme_registry.dart       # ThemeNotifier (StateNotifier) + provider
├── css_parser.dart           # CSS :root {} 解析器
├── glass_wrapper.dart        # BackdropFilter 包装组件
└── builtin/
    ├── light.css             # 从 aaa2 resources/themes/ 同步
    ├── dark.css
    └── glass.css
```

### 3.1 DesignTokens

```dart
@immutable
class DesignTokens {
  // 颜色、字体、间距、圆角、玻璃、动画、布局 — 全部字段
  const DesignTokens({...});

  // 工厂方法
  factory DesignTokens.fromCss(String css);
  factory DesignTokens.light();   // 硬编码兜底
  factory DesignTokens.dark();
  factory DesignTokens.glass();

  // 序列化
  String toCss();

  // 修改
  DesignTokens copyWith({...});

  // 静态访问（从 context 读取 provider）
  static DesignTokens of(BuildContext context);
}
```

### 3.2 CSS 解析器

- 解析 `:root { ... }` 块内的 `--key: value;` 对
- 颜色值：`#RRGGBB`、`#RRGGBBAA`、`rgba(r,g,b,a)`、`transparent` → `Color`
- 尺寸值：`12px` → `double`
- 时间值：`150ms` → `Duration`
- 字符串值：`"Microsoft YaHei"` → `String`
- 未知 key 静默忽略（向前兼容）
- 无效 value 使用内置 fallback

### 3.3 ThemeRegistry + Provider

```dart
final designTokensProvider = StateNotifierProvider<ThemeNotifier, DesignTokens>(
  (ref) => ThemeNotifier(),
);

class ThemeNotifier extends StateNotifier<DesignTokens> {
  Future<void> loadBuiltin(String name);   // light / dark / glass
  Future<void> importCustom(String path);  // 自定义 CSS 文件
  Future<void> reload();
}
```

启动流程：
1. `SharedPreferences` 读取 `themePath`（key: `settings/themePath`）
2. 内置主题（`builtin:light` 等）从 AssetBundle 读取
3. 自定义主题从文件系统读取
4. 解析 CSS → `DesignTokens` → Provider 更新
5. 失败回退到 `DesignTokens.light()` 硬编码

### 3.4 GlassWrapper

```dart
class GlassSurface extends StatelessWidget {
  final Widget child;
  // glassBlur > 0 → ClipRect + BackdropFilter(ImageFilter.blur)
  // 透明度 → 叠加半透明背景色
  // glassBlur == 0 → 普通 Container（无性能开销）
}
```

用于：Sidebar、BottomBar、MessageInputBar、Popup dialogs

### 3.5 ThemeData 同步

`DesignTokens` 变化时，同步更新 `ThemeData`：
- `primaryColor` ← `tokens.primary`
- `scaffoldBackgroundColor` ← `tokens.bgMain`
- `textTheme` ← `tokens.fontBody` 等
- `colorScheme` 从 token 派生

这样 Material Widget（AppBar、Button 等）自动跟随主题，不需要手动传 token。

## 4. 修改清单

| 文件 | 操作 | 说明 |
|------|------|------|
| `lib/core/theme/design_tokens.dart` | 新建 | 令牌数据类 |
| `lib/core/theme/css_parser.dart` | 新建 | CSS 解析器 |
| `lib/core/theme/theme_registry.dart` | 新建 | Provider + ThemeNotifier |
| `lib/core/theme/glass_wrapper.dart` | 新建 | 玻璃效果包装 |
| `lib/core/theme/builtin/*.css` | 新建 (3) | 从 aaa2 复制 |
| `lib/core/theme/app_theme.dart` | 重写 | 从 tokens 构建 ThemeData |
| `lib/presentation/providers/theme_provider.dart` | 重写 | 替换为 DesignTokens provider |
| `lib/app.dart` | 修改 | 使用新 provider |
| `lib/presentation/pages/settings/settings_page.dart` | 修改 | 主题选择下拉 + 导入按钮 |
| `lib/presentation/pages/settings/theme_settings_page.dart` | 新建 | 主题详情/导入页面 |
| `lib/presentation/pages/main_shell.dart` | 修改 | 应用背景色令牌 |
| `pubspec.yaml` | 修改 | 添加 shared_preferences 依赖 |
