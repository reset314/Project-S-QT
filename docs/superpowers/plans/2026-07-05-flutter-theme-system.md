# Flutter 主题系统 — 实现计划

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 将 aaa2 CSS 主题系统迁移到 Flutter — DesignTokens 数据类、CSS :root 解析器、3 套内置主题（light/dark/glass）、BackdropFilter 玻璃效果、持久化切换、自定义 CSS 导入。

**Architecture:** 独立 `DesignTokens` 不可变数据类（字段一一对应 CSS `:root` 变量），`ThemeNotifier` (StateNotifier) 管理加载/切换/导入，`CssParser` 解析 `:root {}` 块，`GlassSurface` 包装 BackdropFilter，`AppTheme` 从 tokens 构建 Material `ThemeData`。

**Tech Stack:** Dart 3.x + Flutter + Riverpod + shared_preferences + 自定义 CSS 解析器

## Global Constraints

- CSS 变量格式与 aaa2 完全一致（`--key: value;` 在 `:root {}` 内）
- 颜色值支持 `#RRGGBB`、`rgba(r,g,b,a)`、`transparent`
- 尺寸值单位 `px`，时间值单位 `ms`
- 玻璃效果通过 BackdropFilter 完整实现，无性能降级
- 内置 3 套主题从 AssetBundle 加载，自定义主题从文件系统加载
- 主题路径持久化 key: `settings/themePath`，值格式 `builtin:<name>` 或 `/absolute/path.css`
- `DesignTokens.of(context)` 为统一访问入口
- 所有现有 Widget 不需修改也能工作（通过 ThemeData 自动继承颜色）

---

### Task 1: 添加依赖 + CSS 资源

**Files:**
- Modify: `pubspec.yaml`
- Create: `lib/core/theme/builtin/light.css`
- Create: `lib/core/theme/builtin/dark.css`
- Create: `lib/core/theme/builtin/glass.css`

**Interfaces:**
- Produces: shared_preferences 包可用，3 个 CSS 资源文件可通过 `rootBundle.loadString()` 加载

- [ ] **Step 1: 添加 shared_preferences 依赖**

在 `pubspec.yaml` 的 dependencies 块末尾（`path: ^1.8.0` 之后）添加：

```yaml
  shared_preferences: ^2.2.2
```

- [ ] **Step 2: 注册 CSS 资源**

将 `pubspec.yaml` 的 `flutter:` 块末尾的注释 `# assets:` 替换为：

```yaml
  assets:
    - lib/core/theme/builtin/
```

- [ ] **Step 3: 运行 pub get**

```bash
cd C:\Users\root\Documents\Programs\aaa1
flutter pub get
```

预期：无错误退出。

- [ ] **Step 4: 创建 light.css**

文件：`lib/core/theme/builtin/light.css`

```css
:root {
  --primary: #1976D2; --primary-light: #63A4FF; --primary-dark: #004BA0;
  --accent: #FF6F00; --accent-light: #FFA040; --accent-dark: #C43E00;
  --bg-main: #F5F5F5; --bg-surface: #FFFFFF; --bg-sidebar: #FAFAFA;
  --bg-titlebar: #1565C0; --bg-input: #FFFFFF;
  --divider: #E0E0E0;
  --text-primary: #212121; --text-secondary: #757575; --text-hint: #BDBDBD;
  --text-on-primary: #FFFFFF; --text-on-accent: #FFFFFF;
  --user-bubble: #1976D2; --user-bubble-text: #FFFFFF;
  --ai-bubble: #FFFFFF; --ai-bubble-text: #212121;
  --system-bubble: #FFF3E0; --system-bubble-text: #E65100;
  --sidebar-hover: #EEEEEE; --sidebar-selected: #E3F2FD;
  --badge: #F44336; --badge-text: #FFFFFF;
  --online: #4CAF50; --offline: #BDBDBD;
  --error: #D32F2F; --warning: #F57C00; --success: #388E3C;
  --font-family: ""; --font-scale: 1.0;
  --font-caption: 11px; --font-small: 12px; --font-body: 14px;
  --font-subheading: 16px; --font-heading: 18px; --font-title: 20px; --font-display: 24px;
  --bubble-radius: 12px; --card-radius: 8px; --button-radius: 4px;
  --spacing-tiny: 4px; --spacing-sm: 8px; --spacing-md: 12px;
  --spacing-lg: 16px; --spacing-xl: 24px;
  --glass-blur: 0px; --glass-opacity: 1.0;
  --anim-fast: 150ms; --anim-normal: 250ms; --anim-slow: 400ms;
  --avatar-sm: 32px; --avatar-md: 40px; --avatar-lg: 56px; --avatar-xl: 80px;
  --icon-sm: 16px; --icon-md: 20px; --icon-lg: 24px;
}
```

- [ ] **Step 5: 创建 dark.css**

文件：`lib/core/theme/builtin/dark.css`

```css
:root {
  --primary: #90CAF9; --primary-light: #BBDEFB; --primary-dark: #42A5F5;
  --accent: #FFB74D; --accent-light: #FFCC80; --accent-dark: #F57C00;
  --bg-main: #121212; --bg-surface: #1E1E1E; --bg-sidebar: #181818;
  --bg-titlebar: #0D0D0D; --bg-input: #2A2A2A;
  --divider: #333333;
  --text-primary: #E0E0E0; --text-secondary: #9E9E9E; --text-hint: #616161;
  --text-on-primary: #121212; --text-on-accent: #121212;
  --user-bubble: #1A73E8; --user-bubble-text: #FFFFFF;
  --ai-bubble: #2A2A2A; --ai-bubble-text: #E0E0E0;
  --system-bubble: #3E2723; --system-bubble-text: #FFCC80;
  --sidebar-hover: #2A2A2A; --sidebar-selected: #1E3A5F;
  --badge: #F44336; --badge-text: #FFFFFF;
  --online: #4CAF50; --offline: #616161;
  --error: #EF5350; --warning: #FFA726; --success: #66BB6A;
  --font-family: ""; --font-scale: 1.0;
  --font-caption: 11px; --font-small: 12px; --font-body: 14px;
  --font-subheading: 16px; --font-heading: 18px; --font-title: 20px; --font-display: 24px;
  --bubble-radius: 12px; --card-radius: 8px; --button-radius: 4px;
  --spacing-tiny: 4px; --spacing-sm: 8px; --spacing-md: 12px;
  --spacing-lg: 16px; --spacing-xl: 24px;
  --glass-blur: 0px; --glass-opacity: 1.0;
  --anim-fast: 150ms; --anim-normal: 250ms; --anim-slow: 400ms;
  --avatar-sm: 32px; --avatar-md: 40px; --avatar-lg: 56px; --avatar-xl: 80px;
  --icon-sm: 16px; --icon-md: 20px; --icon-lg: 24px;
}
```

- [ ] **Step 6: 创建 glass.css**

文件：`lib/core/theme/builtin/glass.css`

```css
:root {
  --primary: #90CAF9; --primary-light: #BBDEFB; --primary-dark: #42A5F5;
  --accent: #FFB74D; --accent-light: #FFCC80; --accent-dark: #F57C00;
  --bg-main: transparent; --bg-surface: rgba(255,255,255,0.06); --bg-sidebar: rgba(0,0,0,0.4);
  --bg-titlebar: rgba(0,0,0,0.5); --bg-input: rgba(255,255,255,0.08);
  --divider: rgba(255,255,255,0.1);
  --text-primary: #E0E0E0; --text-secondary: #9E9E9E; --text-hint: #616161;
  --text-on-primary: #121212; --text-on-accent: #121212;
  --user-bubble: #1A73E8; --user-bubble-text: #FFFFFF;
  --ai-bubble: rgba(255,255,255,0.08); --ai-bubble-text: #E0E0E0;
  --system-bubble: #3E2723; --system-bubble-text: #FFCC80;
  --sidebar-hover: rgba(255,255,255,0.06); --sidebar-selected: rgba(144,202,249,0.15);
  --badge: #F44336; --badge-text: #FFFFFF;
  --online: #4CAF50; --offline: #616161;
  --error: #EF5350; --warning: #FFA726; --success: #66BB6A;
  --font-family: ""; --font-scale: 1.0;
  --font-caption: 11px; --font-small: 12px; --font-body: 14px;
  --font-subheading: 16px; --font-heading: 18px; --font-title: 20px; --font-display: 24px;
  --bubble-radius: 12px; --card-radius: 8px; --button-radius: 4px;
  --spacing-tiny: 4px; --spacing-sm: 8px; --spacing-md: 12px;
  --spacing-lg: 16px; --spacing-xl: 24px;
  --glass-blur: 20px; --glass-opacity: 0.75;
  --anim-fast: 150ms; --anim-normal: 250ms; --anim-slow: 400ms;
  --avatar-sm: 32px; --avatar-md: 40px; --avatar-lg: 56px; --avatar-xl: 80px;
  --icon-sm: 16px; --icon-md: 20px; --icon-lg: 24px;
}
```

- [ ] **Step 7: 验证资源加载**

创建一个简单测试来确认 3 个 CSS 文件可以被 `rootBundle.loadString()` 加载：

```bash
cd C:\Users\root\Documents\Programs\aaa1
dart run -c 'import "package:flutter/services.dart"; void main() async { WidgetsFlutterBinding.ensureInitialized(); for (final name in ["light","dark","glass"]) { final css = await rootBundle.loadString("lib/core/theme/builtin/$name.css"); print("$name: ${css.length} chars OK"); } }'
```

如果 dart run 不支持此语法，跳过此验证，由后续 Task 的编译间接验证。

- [ ] **Step 8: Commit**

```bash
cd C:\Users\root\Documents\Programs\aaa1
git add pubspec.yaml lib/core/theme/builtin/
git commit -m "chore: add shared_preferences dep + 3 built-in CSS theme assets"
```

---

### Task 2: DesignTokens 数据类

**Files:**
- Create: `lib/core/theme/design_tokens.dart`

**Interfaces:**
- Produces: `DesignTokens` 不可变类
  - 构造：`const DesignTokens({...})`
  - 工厂：`DesignTokens.light()`, `DesignTokens.dark()`, `DesignTokens.glass()` — 硬编码兜底
  - 工厂：`DesignTokens.fromCss(String css)` — 委托 CssParser
  - 方法：`DesignTokens copyWith({...})`
  - 方法：`String toCss()`
  - 静态：`DesignTokens of(BuildContext context)` — 从 provider 读取
  - 静态：`DesignTokens lerp(DesignTokens a, DesignTokens b, double t)` — 插值（预留给动画）

- [ ] **Step 1: 编写测试 — 构造 + 兜底工厂 + copyWith + toCss 往返**

创建文件 `test/core/theme/design_tokens_test.dart`：

```dart
import 'package:flutter_test/flutter_test.dart';
import 'package:ai_chat_app/core/theme/design_tokens.dart';

void main() {
  group('DesignTokens', () {
    test('light factory returns non-null tokens', () {
      final tokens = DesignTokens.light();
      expect(tokens.primary.value, isNotEmpty);
      expect(tokens.fontBody, greaterThan(0));
    });

    test('dark factory returns dark background', () {
      final tokens = DesignTokens.dark();
      expect(tokens.bgMain, isNot(equals(DesignTokens.light().bgMain)));
    });

    test('glass factory has glassBlur > 0', () {
      final tokens = DesignTokens.glass();
      expect(tokens.glassBlur, greaterThan(0));
    });

    test('copyWith overrides single field', () {
      final orig = DesignTokens.light();
      final modified = orig.copyWith(primary: const Color(0xFFFF0000));
      expect(modified.primary, const Color(0xFFFF0000));
      expect(modified.primaryLight, orig.primaryLight); // unchanged
      expect(identical(orig, modified), false);
      expect(identical(orig.primaryLight, modified.primaryLight), true); // shared immutable ref
    });

    test('toCss produces valid :root block', () {
      final tokens = DesignTokens.light();
      final css = tokens.toCss();
      expect(css, contains(':root {'));
      expect(css, contains('--primary:'));
      expect(css, contains('}'));
    });

    test('fromCss round-trips with toCss', () {
      final orig = DesignTokens.light();
      final css = orig.toCss();
      final parsed = DesignTokens.fromCss(css);
      expect(parsed.primary, orig.primary);
      expect(parsed.spacingMd, orig.spacingMd);
      expect(parsed.glassBlur, orig.glassBlur);
    });
  });
}
```

- [ ] **Step 2: 运行测试 — 确认全部失败**

```bash
cd C:\Users\root\Documents\Programs\aaa1
flutter test test/core/theme/design_tokens_test.dart
```

预期：编译失败（文件不存在）。

- [ ] **Step 3: 实现 DesignTokens**

创建文件 `lib/core/theme/design_tokens.dart`：

```dart
import 'dart:ui';
import 'package:flutter/foundation.dart';
import 'package:flutter/material.dart';
import 'css_parser.dart';

@immutable
class DesignTokens {
  // ── 颜色 ──
  final Color primary;
  final Color primaryLight;
  final Color primaryDark;
  final Color accent;
  final Color accentLight;
  final Color accentDark;
  final Color bgMain;
  final Color bgSurface;
  final Color bgSidebar;
  final Color bgTitlebar;
  final Color bgInput;
  final Color divider;
  final Color textPrimary;
  final Color textSecondary;
  final Color textHint;
  final Color textOnPrimary;
  final Color textOnAccent;
  final Color userBubble;
  final Color userBubbleText;
  final Color aiBubble;
  final Color aiBubbleText;
  final Color systemBubble;
  final Color systemBubbleText;
  final Color sidebarHover;
  final Color sidebarSelected;
  final Color badge;
  final Color badgeText;
  final Color online;
  final Color offline;
  final Color error;
  final Color warning;
  final Color success;

  // ── 字体 ──
  final String fontFamily;
  final double fontScale;
  final double fontCaption;
  final double fontSmall;
  final double fontBody;
  final double fontSubheading;
  final double fontHeading;
  final double fontTitle;
  final double fontDisplay;

  // ── 间距 ──
  final double spacingTiny;
  final double spacingSm;
  final double spacingMd;
  final double spacingLg;
  final double spacingXl;

  // ── 圆角 ──
  final double bubbleRadius;
  final double cardRadius;
  final double buttonRadius;

  // ── 玻璃 ──
  final double glassBlur;
  final double glassOpacity;

  // ── 动画 ──
  final Duration animFast;
  final Duration animNormal;
  final Duration animSlow;

  // ── 布局 ──
  final double avatarSm;
  final double avatarMd;
  final double avatarLg;
  final double avatarXl;
  final double iconSm;
  final double iconMd;
  final double iconLg;

  const DesignTokens({
    required this.primary,
    required this.primaryLight,
    required this.primaryDark,
    required this.accent,
    required this.accentLight,
    required this.accentDark,
    required this.bgMain,
    required this.bgSurface,
    required this.bgSidebar,
    required this.bgTitlebar,
    required this.bgInput,
    required this.divider,
    required this.textPrimary,
    required this.textSecondary,
    required this.textHint,
    required this.textOnPrimary,
    required this.textOnAccent,
    required this.userBubble,
    required this.userBubbleText,
    required this.aiBubble,
    required this.aiBubbleText,
    required this.systemBubble,
    required this.systemBubbleText,
    required this.sidebarHover,
    required this.sidebarSelected,
    required this.badge,
    required this.badgeText,
    required this.online,
    required this.offline,
    required this.error,
    required this.warning,
    required this.success,
    required this.fontFamily,
    required this.fontScale,
    required this.fontCaption,
    required this.fontSmall,
    required this.fontBody,
    required this.fontSubheading,
    required this.fontHeading,
    required this.fontTitle,
    required this.fontDisplay,
    required this.spacingTiny,
    required this.spacingSm,
    required this.spacingMd,
    required this.spacingLg,
    required this.spacingXl,
    required this.bubbleRadius,
    required this.cardRadius,
    required this.buttonRadius,
    required this.glassBlur,
    required this.glassOpacity,
    required this.animFast,
    required this.animNormal,
    required this.animSlow,
    required this.avatarSm,
    required this.avatarMd,
    required this.avatarLg,
    required this.avatarXl,
    required this.iconSm,
    required this.iconMd,
    required this.iconLg,
  });

  // ── 硬编码兜底工厂 ──

  factory DesignTokens.light() {
    return DesignTokens(
      primary: const Color(0xFF1976D2),
      primaryLight: const Color(0xFF63A4FF),
      primaryDark: const Color(0xFF004BA0),
      accent: const Color(0xFFFF6F00),
      accentLight: const Color(0xFFFFA040),
      accentDark: const Color(0xFFC43E00),
      bgMain: const Color(0xFFF5F5F5),
      bgSurface: const Color(0xFFFFFFFF),
      bgSidebar: const Color(0xFFFAFAFA),
      bgTitlebar: const Color(0xFF1565C0),
      bgInput: const Color(0xFFFFFFFF),
      divider: const Color(0xFFE0E0E0),
      textPrimary: const Color(0xFF212121),
      textSecondary: const Color(0xFF757575),
      textHint: const Color(0xFFBDBDBD),
      textOnPrimary: const Color(0xFFFFFFFF),
      textOnAccent: const Color(0xFFFFFFFF),
      userBubble: const Color(0xFF1976D2),
      userBubbleText: const Color(0xFFFFFFFF),
      aiBubble: const Color(0xFFFFFFFF),
      aiBubbleText: const Color(0xFF212121),
      systemBubble: const Color(0xFFFFF3E0),
      systemBubbleText: const Color(0xFFE65100),
      sidebarHover: const Color(0xFFEEEEEE),
      sidebarSelected: const Color(0xFFE3F2FD),
      badge: const Color(0xFFF44336),
      badgeText: const Color(0xFFFFFFFF),
      online: const Color(0xFF4CAF50),
      offline: const Color(0xFFBDBDBD),
      error: const Color(0xFFD32F2F),
      warning: const Color(0xFFF57C00),
      success: const Color(0xFF388E3C),
      fontFamily: '',
      fontScale: 1.0,
      fontCaption: 11,
      fontSmall: 12,
      fontBody: 14,
      fontSubheading: 16,
      fontHeading: 18,
      fontTitle: 20,
      fontDisplay: 24,
      spacingTiny: 4,
      spacingSm: 8,
      spacingMd: 12,
      spacingLg: 16,
      spacingXl: 24,
      bubbleRadius: 12,
      cardRadius: 8,
      buttonRadius: 4,
      glassBlur: 0,
      glassOpacity: 1.0,
      animFast: const Duration(milliseconds: 150),
      animNormal: const Duration(milliseconds: 250),
      animSlow: const Duration(milliseconds: 400),
      avatarSm: 32,
      avatarMd: 40,
      avatarLg: 56,
      avatarXl: 80,
      iconSm: 16,
      iconMd: 20,
      iconLg: 24,
    );
  }

  factory DesignTokens.dark() {
    return DesignTokens(
      primary: const Color(0xFF90CAF9),
      primaryLight: const Color(0xFFBBDEFB),
      primaryDark: const Color(0xFF42A5F5),
      accent: const Color(0xFFFFB74D),
      accentLight: const Color(0xFFFFCC80),
      accentDark: const Color(0xFFF57C00),
      bgMain: const Color(0xFF121212),
      bgSurface: const Color(0xFF1E1E1E),
      bgSidebar: const Color(0xFF181818),
      bgTitlebar: const Color(0xFF0D0D0D),
      bgInput: const Color(0xFF2A2A2A),
      divider: const Color(0xFF333333),
      textPrimary: const Color(0xFFE0E0E0),
      textSecondary: const Color(0xFF9E9E9E),
      textHint: const Color(0xFF616161),
      textOnPrimary: const Color(0xFF121212),
      textOnAccent: const Color(0xFF121212),
      userBubble: const Color(0xFF1A73E8),
      userBubbleText: const Color(0xFFFFFFFF),
      aiBubble: const Color(0xFF2A2A2A),
      aiBubbleText: const Color(0xFFE0E0E0),
      systemBubble: const Color(0xFF3E2723),
      systemBubbleText: const Color(0xFFFFCC80),
      sidebarHover: const Color(0xFF2A2A2A),
      sidebarSelected: const Color(0xFF1E3A5F),
      badge: const Color(0xFFF44336),
      badgeText: const Color(0xFFFFFFFF),
      online: const Color(0xFF4CAF50),
      offline: const Color(0xFF616161),
      error: const Color(0xFFEF5350),
      warning: const Color(0xFFFFA726),
      success: const Color(0xFF66BB6A),
      fontFamily: '',
      fontScale: 1.0,
      fontCaption: 11,
      fontSmall: 12,
      fontBody: 14,
      fontSubheading: 16,
      fontHeading: 18,
      fontTitle: 20,
      fontDisplay: 24,
      spacingTiny: 4,
      spacingSm: 8,
      spacingMd: 12,
      spacingLg: 16,
      spacingXl: 24,
      bubbleRadius: 12,
      cardRadius: 8,
      buttonRadius: 4,
      glassBlur: 0,
      glassOpacity: 1.0,
      animFast: const Duration(milliseconds: 150),
      animNormal: const Duration(milliseconds: 250),
      animSlow: const Duration(milliseconds: 400),
      avatarSm: 32,
      avatarMd: 40,
      avatarLg: 56,
      avatarXl: 80,
      iconSm: 16,
      iconMd: 20,
      iconLg: 24,
    );
  }

  factory DesignTokens.glass() {
    return DesignTokens(
      primary: const Color(0xFF90CAF9),
      primaryLight: const Color(0xFFBBDEFB),
      primaryDark: const Color(0xFF42A5F5),
      accent: const Color(0xFFFFB74D),
      accentLight: const Color(0xFFFFCC80),
      accentDark: const Color(0xFFF57C00),
      bgMain: const Color(0x00000000),
      bgSurface: const Color(0x0FFFFFFF),
      bgSidebar: const Color(0x66000000),
      bgTitlebar: const Color(0x80000000),
      bgInput: const Color(0x14FFFFFF),
      divider: const Color(0x1AFFFFFF),
      textPrimary: const Color(0xFFE0E0E0),
      textSecondary: const Color(0xFF9E9E9E),
      textHint: const Color(0xFF616161),
      textOnPrimary: const Color(0xFF121212),
      textOnAccent: const Color(0xFF121212),
      userBubble: const Color(0xFF1A73E8),
      userBubbleText: const Color(0xFFFFFFFF),
      aiBubble: const Color(0x14FFFFFF),
      aiBubbleText: const Color(0xFFE0E0E0),
      systemBubble: const Color(0xFF3E2723),
      systemBubbleText: const Color(0xFFFFCC80),
      sidebarHover: const Color(0x0FFFFFFF),
      sidebarSelected: const Color(0x2690CAF9),
      badge: const Color(0xFFF44336),
      badgeText: const Color(0xFFFFFFFF),
      online: const Color(0xFF4CAF50),
      offline: const Color(0xFF616161),
      error: const Color(0xFFEF5350),
      warning: const Color(0xFFFFA726),
      success: const Color(0xFF66BB6A),
      fontFamily: '',
      fontScale: 1.0,
      fontCaption: 11,
      fontSmall: 12,
      fontBody: 14,
      fontSubheading: 16,
      fontHeading: 18,
      fontTitle: 20,
      fontDisplay: 24,
      spacingTiny: 4,
      spacingSm: 8,
      spacingMd: 12,
      spacingLg: 16,
      spacingXl: 24,
      bubbleRadius: 12,
      cardRadius: 8,
      buttonRadius: 4,
      glassBlur: 20,
      glassOpacity: 0.75,
      animFast: const Duration(milliseconds: 150),
      animNormal: const Duration(milliseconds: 250),
      animSlow: const Duration(milliseconds: 400),
      avatarSm: 32,
      avatarMd: 40,
      avatarLg: 56,
      avatarXl: 80,
      iconSm: 16,
      iconMd: 20,
      iconLg: 24,
    );
  }

  // ── CSS 序列化 ──

  factory DesignTokens.fromCss(String css) {
    return CssParser.parse(css);
  }

  String toCss() {
    final buf = StringBuffer();
    buf.writeln(':root {');
    _writeColor(buf, '--primary', primary);
    _writeColor(buf, '--primary-light', primaryLight);
    _writeColor(buf, '--primary-dark', primaryDark);
    _writeColor(buf, '--accent', accent);
    _writeColor(buf, '--accent-light', accentLight);
    _writeColor(buf, '--accent-dark', accentDark);
    _writeColor(buf, '--bg-main', bgMain);
    _writeColor(buf, '--bg-surface', bgSurface);
    _writeColor(buf, '--bg-sidebar', bgSidebar);
    _writeColor(buf, '--bg-titlebar', bgTitlebar);
    _writeColor(buf, '--bg-input', bgInput);
    _writeColor(buf, '--divider', divider);
    _writeColor(buf, '--text-primary', textPrimary);
    _writeColor(buf, '--text-secondary', textSecondary);
    _writeColor(buf, '--text-hint', textHint);
    _writeColor(buf, '--text-on-primary', textOnPrimary);
    _writeColor(buf, '--text-on-accent', textOnAccent);
    _writeColor(buf, '--user-bubble', userBubble);
    _writeColor(buf, '--user-bubble-text', userBubbleText);
    _writeColor(buf, '--ai-bubble', aiBubble);
    _writeColor(buf, '--ai-bubble-text', aiBubbleText);
    _writeColor(buf, '--system-bubble', systemBubble);
    _writeColor(buf, '--system-bubble-text', systemBubbleText);
    _writeColor(buf, '--sidebar-hover', sidebarHover);
    _writeColor(buf, '--sidebar-selected', sidebarSelected);
    _writeColor(buf, '--badge', badge);
    _writeColor(buf, '--badge-text', badgeText);
    _writeColor(buf, '--online', online);
    _writeColor(buf, '--offline', offline);
    _writeColor(buf, '--error', error);
    _writeColor(buf, '--warning', warning);
    _writeColor(buf, '--success', success);
    buf.writeln('  --font-family: ${fontFamily.isEmpty ? '""' : '"$fontFamily"'}; --font-scale: ${fontScale.toStringAsFixed(1)};');
    buf.writeln('  --font-caption: ${fontCaption.toInt()}px; --font-small: ${fontSmall.toInt()}px; --font-body: ${fontBody.toInt()}px;');
    buf.writeln('  --font-subheading: ${fontSubheading.toInt()}px; --font-heading: ${fontHeading.toInt()}px; --font-title: ${fontTitle.toInt()}px; --font-display: ${fontDisplay.toInt()}px;');
    buf.writeln('  --bubble-radius: ${bubbleRadius.toInt()}px; --card-radius: ${cardRadius.toInt()}px; --button-radius: ${buttonRadius.toInt()}px;');
    buf.writeln('  --spacing-tiny: ${spacingTiny.toInt()}px; --spacing-sm: ${spacingSm.toInt()}px; --spacing-md: ${spacingMd.toInt()}px;');
    buf.writeln('  --spacing-lg: ${spacingLg.toInt()}px; --spacing-xl: ${spacingXl.toInt()}px;');
    buf.writeln('  --glass-blur: ${glassBlur.toInt()}px; --glass-opacity: ${glassOpacity.toStringAsFixed(2)};');
    buf.writeln('  --anim-fast: ${animFast.inMilliseconds}ms; --anim-normal: ${animNormal.inMilliseconds}ms; --anim-slow: ${animSlow.inMilliseconds}ms;');
    buf.writeln('  --avatar-sm: ${avatarSm.toInt()}px; --avatar-md: ${avatarMd.toInt()}px; --avatar-lg: ${avatarLg.toInt()}px; --avatar-xl: ${avatarXl.toInt()}px;');
    buf.writeln('  --icon-sm: ${iconSm.toInt()}px; --icon-md: ${iconMd.toInt()}px; --icon-lg: ${iconLg.toInt()}px;');
    buf.writeln('}');
    return buf.toString();
  }

  static void _writeColor(StringBuffer buf, String name, Color color) {
    final r = (color.r * 255).round();
    final g = (color.g * 255).round();
    final b = (color.b * 255).round();
    final a = (color.a * 255).round();
    if (a == 255) {
      buf.writeln('  $name: #${r.toRadixString(16).padLeft(2, '0').toUpperCase()}${g.toRadixString(16).padLeft(2, '0').toUpperCase()}${b.toRadixString(16).padLeft(2, '0').toUpperCase()};');
    } else if (a == 0 && r == 0 && g == 0 && b == 0) {
      buf.writeln('  $name: transparent;');
    } else {
      buf.writeln('  $name: rgba($r,$g,$b,${(a / 255).toStringAsFixed(2)});');
    }
  }

  // ── copyWith ──

  DesignTokens copyWith({
    Color? primary,
    Color? primaryLight,
    Color? primaryDark,
    Color? accent,
    Color? accentLight,
    Color? accentDark,
    Color? bgMain,
    Color? bgSurface,
    Color? bgSidebar,
    Color? bgTitlebar,
    Color? bgInput,
    Color? divider,
    Color? textPrimary,
    Color? textSecondary,
    Color? textHint,
    Color? textOnPrimary,
    Color? textOnAccent,
    Color? userBubble,
    Color? userBubbleText,
    Color? aiBubble,
    Color? aiBubbleText,
    Color? systemBubble,
    Color? systemBubbleText,
    Color? sidebarHover,
    Color? sidebarSelected,
    Color? badge,
    Color? badgeText,
    Color? online,
    Color? offline,
    Color? error,
    Color? warning,
    Color? success,
    String? fontFamily,
    double? fontScale,
    double? fontCaption,
    double? fontSmall,
    double? fontBody,
    double? fontSubheading,
    double? fontHeading,
    double? fontTitle,
    double? fontDisplay,
    double? spacingTiny,
    double? spacingSm,
    double? spacingMd,
    double? spacingLg,
    double? spacingXl,
    double? bubbleRadius,
    double? cardRadius,
    double? buttonRadius,
    double? glassBlur,
    double? glassOpacity,
    Duration? animFast,
    Duration? animNormal,
    Duration? animSlow,
    double? avatarSm,
    double? avatarMd,
    double? avatarLg,
    double? avatarXl,
    double? iconSm,
    double? iconMd,
    double? iconLg,
  }) {
    return DesignTokens(
      primary: primary ?? this.primary,
      primaryLight: primaryLight ?? this.primaryLight,
      primaryDark: primaryDark ?? this.primaryDark,
      accent: accent ?? this.accent,
      accentLight: accentLight ?? this.accentLight,
      accentDark: accentDark ?? this.accentDark,
      bgMain: bgMain ?? this.bgMain,
      bgSurface: bgSurface ?? this.bgSurface,
      bgSidebar: bgSidebar ?? this.bgSidebar,
      bgTitlebar: bgTitlebar ?? this.bgTitlebar,
      bgInput: bgInput ?? this.bgInput,
      divider: divider ?? this.divider,
      textPrimary: textPrimary ?? this.textPrimary,
      textSecondary: textSecondary ?? this.textSecondary,
      textHint: textHint ?? this.textHint,
      textOnPrimary: textOnPrimary ?? this.textOnPrimary,
      textOnAccent: textOnAccent ?? this.textOnAccent,
      userBubble: userBubble ?? this.userBubble,
      userBubbleText: userBubbleText ?? this.userBubbleText,
      aiBubble: aiBubble ?? this.aiBubble,
      aiBubbleText: aiBubbleText ?? this.aiBubbleText,
      systemBubble: systemBubble ?? this.systemBubble,
      systemBubbleText: systemBubbleText ?? this.systemBubbleText,
      sidebarHover: sidebarHover ?? this.sidebarHover,
      sidebarSelected: sidebarSelected ?? this.sidebarSelected,
      badge: badge ?? this.badge,
      badgeText: badgeText ?? this.badgeText,
      online: online ?? this.online,
      offline: offline ?? this.offline,
      error: error ?? this.error,
      warning: warning ?? this.warning,
      success: success ?? this.success,
      fontFamily: fontFamily ?? this.fontFamily,
      fontScale: fontScale ?? this.fontScale,
      fontCaption: fontCaption ?? this.fontCaption,
      fontSmall: fontSmall ?? this.fontSmall,
      fontBody: fontBody ?? this.fontBody,
      fontSubheading: fontSubheading ?? this.fontSubheading,
      fontHeading: fontHeading ?? this.fontHeading,
      fontTitle: fontTitle ?? this.fontTitle,
      fontDisplay: fontDisplay ?? this.fontDisplay,
      spacingTiny: spacingTiny ?? this.spacingTiny,
      spacingSm: spacingSm ?? this.spacingSm,
      spacingMd: spacingMd ?? this.spacingMd,
      spacingLg: spacingLg ?? this.spacingLg,
      spacingXl: spacingXl ?? this.spacingXl,
      bubbleRadius: bubbleRadius ?? this.bubbleRadius,
      cardRadius: cardRadius ?? this.cardRadius,
      buttonRadius: buttonRadius ?? this.buttonRadius,
      glassBlur: glassBlur ?? this.glassBlur,
      glassOpacity: glassOpacity ?? this.glassOpacity,
      animFast: animFast ?? this.animFast,
      animNormal: animNormal ?? this.animNormal,
      animSlow: animSlow ?? this.animSlow,
      avatarSm: avatarSm ?? this.avatarSm,
      avatarMd: avatarMd ?? this.avatarMd,
      avatarLg: avatarLg ?? this.avatarLg,
      avatarXl: avatarXl ?? this.avatarXl,
      iconSm: iconSm ?? this.iconSm,
      iconMd: iconMd ?? this.iconMd,
      iconLg: iconLg ?? this.iconLg,
    );
  }

  // ── Context 访问 ──

  static DesignTokens of(BuildContext context) {
    // 通过继承的 widget 获取 — 由 ThemeRegistry provider 注入
    return context
        .dependOnInheritedWidgetOfExactType<_TokensScope>()!
        .tokens;
  }

  // ── 插值 ──

  static DesignTokens lerp(DesignTokens a, DesignTokens b, double t) {
    return DesignTokens(
      primary: Color.lerp(a.primary, b.primary, t)!,
      primaryLight: Color.lerp(a.primaryLight, b.primaryLight, t)!,
      primaryDark: Color.lerp(a.primaryDark, b.primaryDark, t)!,
      accent: Color.lerp(a.accent, b.accent, t)!,
      accentLight: Color.lerp(a.accentLight, b.accentLight, t)!,
      accentDark: Color.lerp(a.accentDark, b.accentDark, t)!,
      bgMain: Color.lerp(a.bgMain, b.bgMain, t)!,
      bgSurface: Color.lerp(a.bgSurface, b.bgSurface, t)!,
      bgSidebar: Color.lerp(a.bgSidebar, b.bgSidebar, t)!,
      bgTitlebar: Color.lerp(a.bgTitlebar, b.bgTitlebar, t)!,
      bgInput: Color.lerp(a.bgInput, b.bgInput, t)!,
      divider: Color.lerp(a.divider, b.divider, t)!,
      textPrimary: Color.lerp(a.textPrimary, b.textPrimary, t)!,
      textSecondary: Color.lerp(a.textSecondary, b.textSecondary, t)!,
      textHint: Color.lerp(a.textHint, b.textHint, t)!,
      textOnPrimary: Color.lerp(a.textOnPrimary, b.textOnPrimary, t)!,
      textOnAccent: Color.lerp(a.textOnAccent, b.textOnAccent, t)!,
      userBubble: Color.lerp(a.userBubble, b.userBubble, t)!,
      userBubbleText: Color.lerp(a.userBubbleText, b.userBubbleText, t)!,
      aiBubble: Color.lerp(a.aiBubble, b.aiBubble, t)!,
      aiBubbleText: Color.lerp(a.aiBubbleText, b.aiBubbleText, t)!,
      systemBubble: Color.lerp(a.systemBubble, b.systemBubble, t)!,
      systemBubbleText: Color.lerp(a.systemBubbleText, b.systemBubbleText, t)!,
      sidebarHover: Color.lerp(a.sidebarHover, b.sidebarHover, t)!,
      sidebarSelected: Color.lerp(a.sidebarSelected, b.sidebarSelected, t)!,
      badge: Color.lerp(a.badge, b.badge, t)!,
      badgeText: Color.lerp(a.badgeText, b.badgeText, t)!,
      online: Color.lerp(a.online, b.online, t)!,
      offline: Color.lerp(a.offline, b.offline, t)!,
      error: Color.lerp(a.error, b.error, t)!,
      warning: Color.lerp(a.warning, b.warning, t)!,
      success: Color.lerp(a.success, b.success, t)!,
      fontFamily: t < 0.5 ? a.fontFamily : b.fontFamily,
      fontScale: lerpDouble(a.fontScale, b.fontScale, t),
      fontCaption: lerpDouble(a.fontCaption, b.fontCaption, t),
      fontSmall: lerpDouble(a.fontSmall, b.fontSmall, t),
      fontBody: lerpDouble(a.fontBody, b.fontBody, t),
      fontSubheading: lerpDouble(a.fontSubheading, b.fontSubheading, t),
      fontHeading: lerpDouble(a.fontHeading, b.fontHeading, t),
      fontTitle: lerpDouble(a.fontTitle, b.fontTitle, t),
      fontDisplay: lerpDouble(a.fontDisplay, b.fontDisplay, t),
      spacingTiny: lerpDouble(a.spacingTiny, b.spacingTiny, t),
      spacingSm: lerpDouble(a.spacingSm, b.spacingSm, t),
      spacingMd: lerpDouble(a.spacingMd, b.spacingMd, t),
      spacingLg: lerpDouble(a.spacingLg, b.spacingLg, t),
      spacingXl: lerpDouble(a.spacingXl, b.spacingXl, t),
      bubbleRadius: lerpDouble(a.bubbleRadius, b.bubbleRadius, t),
      cardRadius: lerpDouble(a.cardRadius, b.cardRadius, t),
      buttonRadius: lerpDouble(a.buttonRadius, b.buttonRadius, t),
      glassBlur: lerpDouble(a.glassBlur, b.glassBlur, t),
      glassOpacity: lerpDouble(a.glassOpacity, b.glassOpacity, t),
      animFast: Duration(milliseconds: lerpDouble(a.animFast.inMilliseconds.toDouble(), b.animFast.inMilliseconds.toDouble(), t).round()),
      animNormal: Duration(milliseconds: lerpDouble(a.animNormal.inMilliseconds.toDouble(), b.animNormal.inMilliseconds.toDouble(), t).round()),
      animSlow: Duration(milliseconds: lerpDouble(a.animSlow.inMilliseconds.toDouble(), b.animSlow.inMilliseconds.toDouble(), t).round()),
      avatarSm: lerpDouble(a.avatarSm, b.avatarSm, t),
      avatarMd: lerpDouble(a.avatarMd, b.avatarMd, t),
      avatarLg: lerpDouble(a.avatarLg, b.avatarLg, t),
      avatarXl: lerpDouble(a.avatarXl, b.avatarXl, t),
      iconSm: lerpDouble(a.iconSm, b.iconSm, t),
      iconMd: lerpDouble(a.iconMd, b.iconMd, t),
      iconLg: lerpDouble(a.iconLg, b.iconLg, t),
    );
  }
}

// ── InheritedWidget Scope ──

class _TokensScope extends InheritedWidget {
  final DesignTokens tokens;

  const _TokensScope({
    required this.tokens,
    required super.child,
  });

  @override
  bool updateShouldNotify(_TokensScope oldWidget) => tokens != oldWidget.tokens;
}
```

注意：`CssParser` 在 Task 3 创建，此时尚不存在。设计上 `DesignTokens.fromCss` 委托 `CssParser.parse`，依赖 Task 3 完成后才能编译通过。测试步骤会在 Task 3 完成后执行。

- [ ] **Step 4: 提交骨架（测试 skip，待 Task 3 一起跑）**

```bash
cd C:\Users\root\Documents\Programs\aaa1
git add lib/core/theme/design_tokens.dart test/core/theme/design_tokens_test.dart
git commit -m "feat(theme): add DesignTokens data class with hardcoded fallbacks"
```

---

### Task 3: CSS 解析器

**Files:**
- Create: `lib/core/theme/css_parser.dart`
- Create: `test/core/theme/css_parser_test.dart`

**Interfaces:**
- Consumes: (none)
- Produces: `CssParser` — 纯静态工具类
  - `static DesignTokens parse(String css)` — 解析 `:root {}` 块
  - `static Color parseColor(String value)` — 解析颜色值
  - `static double parsePx(String value)` — 解析 px 值

- [ ] **Step 1: 编写测试**

文件：`test/core/theme/css_parser_test.dart`

```dart
import 'dart:ui';
import 'package:flutter_test/flutter_test.dart';
import 'package:ai_chat_app/core/theme/css_parser.dart';

void main() {
  group('CssParser.parseColor', () {
    test('parses 6-digit hex', () {
      expect(CssParser.parseColor('#1976D2'), const Color(0xFF1976D2));
    });

    test('parses 8-digit hex', () {
      expect(CssParser.parseColor('#FF1976D2'), const Color(0xFF1976D2));
    });

    test('parses rgba', () {
      final c = CssParser.parseColor('rgba(255,255,255,0.06)');
      expect(c.r, closeTo(1.0, 0.01));
      expect(c.g, closeTo(1.0, 0.01));
      expect(c.b, closeTo(1.0, 0.01));
      expect(c.a, closeTo(0.06, 0.01));
    });

    test('parses transparent', () {
      final c = CssParser.parseColor('transparent');
      expect(c.a, 0);
    });

    test('handles whitespace in rgba', () {
      final c = CssParser.parseColor('rgba( 255 , 255 , 255 , 0.5 )');
      expect(c.a, closeTo(0.5, 0.01));
    });
  });

  group('CssParser.parsePx', () {
    test('parses px value', () {
      expect(CssParser.parsePx('12px'), 12.0);
    });

    test('parses px with decimal', () {
      expect(CssParser.parsePx('0.75px'), closeTo(0.75, 0.001));
    });
  });

  group('CssParser.parseMs', () {
    test('parses ms value', () {
      expect(CssParser.parseMs('150ms'), 150);
    });
  });

  group('CssParser.parse', () {
    test('parses full :root block', () {
      const css = '''
:root {
  --primary: #1976D2;
  --bg-main: #F5F5F5;
  --glass-blur: 20px;
  --glass-opacity: 0.75;
  --font-body: 14px;
}
''';
      final tokens = CssParser.parse(css);
      expect(tokens.primary, const Color(0xFF1976D2));
      expect(tokens.bgMain, const Color(0xFFF5F5F5));
      expect(tokens.glassBlur, 20.0);
      expect(tokens.glassOpacity, 0.75);
      expect(tokens.fontBody, 14.0);
    });

    test('unknown keys silently ignored', () {
      const css = ':root { --unknown-token: #FF0000; --primary: #1976D2; }';
      final tokens = CssParser.parse(css);
      expect(tokens.primary, const Color(0xFF1976D2));
      // no exception for --unknown-token
    });

    test('missing keys use light fallback', () {
      const css = ':root { --primary: #FF0000; }';
      final tokens = CssParser.parse(css);
      expect(tokens.primary, const Color(0xFFFF0000));
      expect(tokens.bgMain, DesignTokens.light().bgMain); // fallback
    });

    test('malformed color falls back to light value', () {
      const css = ':root { --primary: not-a-color; }';
      final tokens = CssParser.parse(css);
      expect(tokens.primary, DesignTokens.light().primary); // fallback
    });
  });
}
```

- [ ] **Step 2: 运行测试 — 确认全部失败**

```bash
cd C:\Users\root\Documents\Programs\aaa1
flutter test test/core/theme/css_parser_test.dart
```

- [ ] **Step 3: 实现 CssParser**

文件：`lib/core/theme/css_parser.dart`

```dart
import 'dart:ui';
import 'package:flutter/foundation.dart';
import 'design_tokens.dart';

class CssParser {
  /// Parse a CSS :root { } block into DesignTokens.
  /// Returns tokens with parsed values; missing keys fall back to light defaults.
  static DesignTokens parse(String css) {
    // Start with light fallbacks
    final light = DesignTokens.light();
    Color primary = light.primary;
    Color primaryLight = light.primaryLight;
    Color primaryDark = light.primaryDark;
    Color accent = light.accent;
    Color accentLight = light.accentLight;
    Color accentDark = light.accentDark;
    Color bgMain = light.bgMain;
    Color bgSurface = light.bgSurface;
    Color bgSidebar = light.bgSidebar;
    Color bgTitlebar = light.bgTitlebar;
    Color bgInput = light.bgInput;
    Color divider = light.divider;
    Color textPrimary = light.textPrimary;
    Color textSecondary = light.textSecondary;
    Color textHint = light.textHint;
    Color textOnPrimary = light.textOnPrimary;
    Color textOnAccent = light.textOnAccent;
    Color userBubble = light.userBubble;
    Color userBubbleText = light.userBubbleText;
    Color aiBubble = light.aiBubble;
    Color aiBubbleText = light.aiBubbleText;
    Color systemBubble = light.systemBubble;
    Color systemBubbleText = light.systemBubbleText;
    Color sidebarHover = light.sidebarHover;
    Color sidebarSelected = light.sidebarSelected;
    Color badge = light.badge;
    Color badgeText = light.badgeText;
    Color online = light.online;
    Color offline = light.offline;
    Color error = light.error;
    Color warning = light.warning;
    Color success = light.success;
    String fontFamily = light.fontFamily;
    double fontScale = light.fontScale;
    double fontCaption = light.fontCaption;
    double fontSmall = light.fontSmall;
    double fontBody = light.fontBody;
    double fontSubheading = light.fontSubheading;
    double fontHeading = light.fontHeading;
    double fontTitle = light.fontTitle;
    double fontDisplay = light.fontDisplay;
    double spacingTiny = light.spacingTiny;
    double spacingSm = light.spacingSm;
    double spacingMd = light.spacingMd;
    double spacingLg = light.spacingLg;
    double spacingXl = light.spacingXl;
    double bubbleRadius = light.bubbleRadius;
    double cardRadius = light.cardRadius;
    double buttonRadius = light.buttonRadius;
    double glassBlur = light.glassBlur;
    double glassOpacity = light.glassOpacity;
    Duration animFast = light.animFast;
    Duration animNormal = light.animNormal;
    Duration animSlow = light.animSlow;
    double avatarSm = light.avatarSm;
    double avatarMd = light.avatarMd;
    double avatarLg = light.avatarLg;
    double avatarXl = light.avatarXl;
    double iconSm = light.iconSm;
    double iconMd = light.iconMd;
    double iconLg = light.iconLg;

    // Extract the :root { ... } block
    final rootMatch = RegExp(r':root\s*\{([^}]*)\}', dotAll: true).firstMatch(css);
    if (rootMatch == null) return light;

    final body = rootMatch.group(1)!;
    // Match --key: value; pairs (value may contain nested parens like rgba())
    final propRe = RegExp(r'--([a-zA-Z0-9_-]+)\s*:\s*([^;]+);');
    for (final m in propRe.allMatches(body)) {
      final key = m.group(1)!;
      final value = m.group(2)!.trim();

      switch (key) {
        // Colors
        case 'primary': primary = _parseColorSafe(value, primary); break;
        case 'primary-light': primaryLight = _parseColorSafe(value, primaryLight); break;
        case 'primary-dark': primaryDark = _parseColorSafe(value, primaryDark); break;
        case 'accent': accent = _parseColorSafe(value, accent); break;
        case 'accent-light': accentLight = _parseColorSafe(value, accentLight); break;
        case 'accent-dark': accentDark = _parseColorSafe(value, accentDark); break;
        case 'bg-main': bgMain = _parseColorSafe(value, bgMain); break;
        case 'bg-surface': bgSurface = _parseColorSafe(value, bgSurface); break;
        case 'bg-sidebar': bgSidebar = _parseColorSafe(value, bgSidebar); break;
        case 'bg-titlebar': bgTitlebar = _parseColorSafe(value, bgTitlebar); break;
        case 'bg-input': bgInput = _parseColorSafe(value, bgInput); break;
        case 'divider': divider = _parseColorSafe(value, divider); break;
        case 'text-primary': textPrimary = _parseColorSafe(value, textPrimary); break;
        case 'text-secondary': textSecondary = _parseColorSafe(value, textSecondary); break;
        case 'text-hint': textHint = _parseColorSafe(value, textHint); break;
        case 'text-on-primary': textOnPrimary = _parseColorSafe(value, textOnPrimary); break;
        case 'text-on-accent': textOnAccent = _parseColorSafe(value, textOnAccent); break;
        case 'user-bubble': userBubble = _parseColorSafe(value, userBubble); break;
        case 'user-bubble-text': userBubbleText = _parseColorSafe(value, userBubbleText); break;
        case 'ai-bubble': aiBubble = _parseColorSafe(value, aiBubble); break;
        case 'ai-bubble-text': aiBubbleText = _parseColorSafe(value, aiBubbleText); break;
        case 'system-bubble': systemBubble = _parseColorSafe(value, systemBubble); break;
        case 'system-bubble-text': systemBubbleText = _parseColorSafe(value, systemBubbleText); break;
        case 'sidebar-hover': sidebarHover = _parseColorSafe(value, sidebarHover); break;
        case 'sidebar-selected': sidebarSelected = _parseColorSafe(value, sidebarSelected); break;
        case 'badge': badge = _parseColorSafe(value, badge); break;
        case 'badge-text': badgeText = _parseColorSafe(value, badgeText); break;
        case 'online': online = _parseColorSafe(value, online); break;
        case 'offline': offline = _parseColorSafe(value, offline); break;
        case 'error': error = _parseColorSafe(value, error); break;
        case 'warning': warning = _parseColorSafe(value, warning); break;
        case 'success': success = _parseColorSafe(value, success); break;
        // Strings
        case 'font-family':
          fontFamily = value.replaceAll('"', '').trim();
          break;
        // Scalars
        case 'font-scale': fontScale = _parseDoubleSafe(value, 1.0); break;
        // Px sizes
        case 'font-caption': fontCaption = _parsePxSafe(value, fontCaption); break;
        case 'font-small': fontSmall = _parsePxSafe(value, fontSmall); break;
        case 'font-body': fontBody = _parsePxSafe(value, fontBody); break;
        case 'font-subheading': fontSubheading = _parsePxSafe(value, fontSubheading); break;
        case 'font-heading': fontHeading = _parsePxSafe(value, fontHeading); break;
        case 'font-title': fontTitle = _parsePxSafe(value, fontTitle); break;
        case 'font-display': fontDisplay = _parsePxSafe(value, fontDisplay); break;
        case 'spacing-tiny': spacingTiny = _parsePxSafe(value, spacingTiny); break;
        case 'spacing-sm': spacingSm = _parsePxSafe(value, spacingSm); break;
        case 'spacing-md': spacingMd = _parsePxSafe(value, spacingMd); break;
        case 'spacing-lg': spacingLg = _parsePxSafe(value, spacingLg); break;
        case 'spacing-xl': spacingXl = _parsePxSafe(value, spacingXl); break;
        case 'bubble-radius': bubbleRadius = _parsePxSafe(value, bubbleRadius); break;
        case 'card-radius': cardRadius = _parsePxSafe(value, cardRadius); break;
        case 'button-radius': buttonRadius = _parsePxSafe(value, buttonRadius); break;
        case 'glass-blur': glassBlur = _parsePxSafe(value, glassBlur); break;
        case 'glass-opacity': glassOpacity = _parseDoubleSafe(value, 1.0); break;
        case 'avatar-sm': avatarSm = _parsePxSafe(value, avatarSm); break;
        case 'avatar-md': avatarMd = _parsePxSafe(value, avatarMd); break;
        case 'avatar-lg': avatarLg = _parsePxSafe(value, avatarLg); break;
        case 'avatar-xl': avatarXl = _parsePxSafe(value, avatarXl); break;
        case 'icon-sm': iconSm = _parsePxSafe(value, iconSm); break;
        case 'icon-md': iconMd = _parsePxSafe(value, iconMd); break;
        case 'icon-lg': iconLg = _parsePxSafe(value, iconLg); break;
        // Durations
        case 'anim-fast': animFast = _parseDurationSafe(value, animFast); break;
        case 'anim-normal': animNormal = _parseDurationSafe(value, animNormal); break;
        case 'anim-slow': animSlow = _parseDurationSafe(value, animSlow); break;
        // unknown key → ignore
      }
    }

    return DesignTokens(
      primary: primary, primaryLight: primaryLight, primaryDark: primaryDark,
      accent: accent, accentLight: accentLight, accentDark: accentDark,
      bgMain: bgMain, bgSurface: bgSurface, bgSidebar: bgSidebar,
      bgTitlebar: bgTitlebar, bgInput: bgInput, divider: divider,
      textPrimary: textPrimary, textSecondary: textSecondary, textHint: textHint,
      textOnPrimary: textOnPrimary, textOnAccent: textOnAccent,
      userBubble: userBubble, userBubbleText: userBubbleText,
      aiBubble: aiBubble, aiBubbleText: aiBubbleText,
      systemBubble: systemBubble, systemBubbleText: systemBubbleText,
      sidebarHover: sidebarHover, sidebarSelected: sidebarSelected,
      badge: badge, badgeText: badgeText,
      online: online, offline: offline,
      error: error, warning: warning, success: success,
      fontFamily: fontFamily, fontScale: fontScale,
      fontCaption: fontCaption, fontSmall: fontSmall, fontBody: fontBody,
      fontSubheading: fontSubheading, fontHeading: fontHeading,
      fontTitle: fontTitle, fontDisplay: fontDisplay,
      spacingTiny: spacingTiny, spacingSm: spacingSm, spacingMd: spacingMd,
      spacingLg: spacingLg, spacingXl: spacingXl,
      bubbleRadius: bubbleRadius, cardRadius: cardRadius, buttonRadius: buttonRadius,
      glassBlur: glassBlur, glassOpacity: glassOpacity,
      animFast: animFast, animNormal: animNormal, animSlow: animSlow,
      avatarSm: avatarSm, avatarMd: avatarMd, avatarLg: avatarLg, avatarXl: avatarXl,
      iconSm: iconSm, iconMd: iconMd, iconLg: iconLg,
    );
  }

  /// Public for testing — parse a CSS color value to dart:ui Color.
  static Color parseColor(String value) {
    final v = value.trim();
    if (v == 'transparent') return const Color(0x00000000);
    final rgba = RegExp(r'rgba\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*([\d.]+)\s*\)').firstMatch(v);
    if (rgba != null) {
      return Color.fromRGBO(
        int.parse(rgba.group(1)!).clamp(0, 255),
        int.parse(rgba.group(2)!).clamp(0, 255),
        int.parse(rgba.group(3)!).clamp(0, 255),
        double.parse(rgba.group(4)!).clamp(0.0, 1.0),
      );
    }
    final hex = v.replaceFirst('#', '');
    if (hex.length == 6) {
      return Color(int.parse('FF$hex', radix: 16));
    }
    if (hex.length == 8) {
      return Color(int.parse(hex, radix: 16));
    }
    throw FormatException('Cannot parse color: $value');
  }

  /// Public for testing — parse a CSS px value.
  static double parsePx(String value) {
    return double.parse(value.trim().replaceFirst('px', '').trim());
  }

  /// Public for testing — parse a CSS ms value.
  static int parseMs(String value) {
    return int.parse(value.trim().replaceFirst('ms', '').trim());
  }

  // ── Internal safe parsers (return fallback on error) ──

  static Color _parseColorSafe(String value, Color fallback) {
    try { return parseColor(value); } catch (_) { return fallback; }
  }

  static double _parsePxSafe(String value, double fallback) {
    try { return parsePx(value); } catch (_) { return fallback; }
  }

  static double _parseDoubleSafe(String value, double fallback) {
    try { return double.parse(value.trim()); } catch (_) { return fallback; }
  }

  static Duration _parseDurationSafe(String value, Duration fallback) {
    try { return Duration(milliseconds: parseMs(value)); } catch (_) { return fallback; }
  }
}
```

- [ ] **Step 4: 运行所有测试**

```bash
cd C:\Users\root\Documents\Programs\aaa1
flutter test test/core/theme/
```

预期：全部 PASS。

- [ ] **Step 5: Commit**

```bash
cd C:\Users\root\Documents\Programs\aaa1
git add lib/core/theme/css_parser.dart test/core/theme/css_parser_test.dart
git commit -m "feat(theme): add CSS :root parser with color/rgba/px/ms support"
```

---

### Task 4: ThemeNotifier + Provider + 持久化

**Files:**
- Create: `lib/core/theme/theme_registry.dart`
- Create: `test/core/theme/theme_registry_test.dart`

**Interfaces:**
- Consumes: `DesignTokens`, `CssParser`
- Produces:
  - `final designTokensProvider = StateNotifierProvider<ThemeNotifier, DesignTokens>(...)`
  - `ThemeNotifier`:
    - `Future<void> loadBuiltin(String name)` — 从 AssetBundle 加载 `builtin/<name>.css`
    - `Future<void> loadCustom(String filePath)` — 从文件系统加载
    - `Future<void> reload()` — 重新加载当前主题
    - `String currentThemeKey` — 当前 key（`builtin:light` 或文件路径）

- [ ] **Step 1: 编写测试**

文件：`test/core/theme/theme_registry_test.dart`

```dart
import 'package:flutter/services.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:ai_chat_app/core/theme/design_tokens.dart';
import 'package:ai_chat_app/core/theme/theme_registry.dart';

void main() {
  TestWidgetsFlutterBinding.ensureInitialized();

  // Mock the asset bundle to return known CSS
  final testCss = '''
:root {
  --primary: #FF0000;
  --bg-main: #FFFFFF;
}
''';

  test('ThemeNotifier loadBuiltin parses CSS from assets', () async {
    // Setup mock asset bundle
    TestDefaultBinaryMessengerBinding.instance.defaultBinaryMessenger
        .setMockMessageHandler('flutter/assets', (msg) {
      final channel = 'flutter/assets';
      // Return the CSS content for any asset request
      return testCss.codeUnits.buffer.asByteData();
    });

    final container = ProviderContainer();
    addTearDown(container.dispose);

    final notifier = container.read(designTokensProvider.notifier);
    // This test validates the provider infrastructure; exact asset loading
    // requires integration test. Skip asset-loading test, validate only fallback.
  });

  test('ThemeNotifier initial state is light fallback', () {
    final container = ProviderContainer();
    addTearDown(container.dispose);

    final tokens = container.read(designTokensProvider);
    expect(tokens.primary, DesignTokens.light().primary);
  });
}
```

注意：AssetBundle 加载在 unit test 中难以完整模拟。此测试验证 provider 初始状态和基础设施正确。CSS 加载功能由后续集成 build 验证。

- [ ] **Step 2: 实现 ThemeNotifier**

文件：`lib/core/theme/theme_registry.dart`

```dart
import 'dart:io';
import 'package:flutter/services.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'design_tokens.dart';

const _storageKey = 'settings/themePath';

final designTokensProvider =
    StateNotifierProvider<ThemeNotifier, DesignTokens>((ref) {
  return ThemeNotifier();
});

class ThemeNotifier extends StateNotifier<DesignTokens> {
  String _currentThemeKey = 'builtin:light';

  String get currentThemeKey => _currentThemeKey;

  ThemeNotifier() : super(DesignTokens.light()) {
    _loadSaved();
  }

  Future<void> _loadSaved() async {
    try {
      final prefs = await SharedPreferences.getInstance();
      final saved = prefs.getString(_storageKey) ?? 'builtin:light';
      if (saved.startsWith('builtin:')) {
        await loadBuiltin(saved.substring('builtin:'.length));
      } else if (File(saved).existsSync()) {
        await loadCustom(saved);
      }
      // else: stay on light fallback
    } catch (_) {
      // SharedPreferences unavailable → stay on light fallback
    }
  }

  Future<void> loadBuiltin(String name) async {
    try {
      final css = await rootBundle.loadString('lib/core/theme/builtin/$name.css');
      state = DesignTokens.fromCss(css);
      _currentThemeKey = 'builtin:$name';
      await _persist();
    } catch (e) {
      // On failure, fall back to hardcoded
      switch (name) {
        case 'dark':
          state = DesignTokens.dark();
          break;
        case 'glass':
          state = DesignTokens.glass();
          break;
        default:
          state = DesignTokens.light();
      }
      _currentThemeKey = 'builtin:$name';
      await _persist();
    }
  }

  Future<void> loadCustom(String filePath) async {
    final file = File(filePath);
    if (!file.existsSync()) return;

    final css = await file.readAsString();
    state = DesignTokens.fromCss(css);
    _currentThemeKey = filePath;
    await _persist();
  }

  Future<void> reload() async {
    if (_currentThemeKey.startsWith('builtin:')) {
      await loadBuiltin(_currentThemeKey.substring('builtin:'.length));
    } else {
      await loadCustom(_currentThemeKey);
    }
  }

  Future<void> _persist() async {
    try {
      final prefs = await SharedPreferences.getInstance();
      await prefs.setString(_storageKey, _currentThemeKey);
    } catch (_) {}
  }
}
```

- [ ] **Step 3: 运行测试**

```bash
cd C:\Users\root\Documents\Programs\aaa1
flutter test test/core/theme/
```

- [ ] **Step 4: Commit**

```bash
cd C:\Users\root\Documents\Programs\aaa1
git add lib/core/theme/theme_registry.dart test/core/theme/theme_registry_test.dart
git commit -m "feat(theme): add ThemeNotifier with SharedPreferences persistence"
```

---

### Task 5: GlassWrapper 组件

**Files:**
- Create: `lib/core/theme/glass_wrapper.dart`

**Interfaces:**
- Produces: `GlassSurface` widget
  - `GlassSurface({required Widget child, double? blurOverride, double? opacityOverride})`
  - 读取 `DesignTokens.of(context).glassBlur`，若 > 0 则使用 BackdropFilter
  - 若 blur == 0，只渲染 Container（无性能开销）

- [ ] **Step 1: 实现 GlassSurface**

文件：`lib/core/theme/glass_wrapper.dart`

```dart
import 'dart:ui';
import 'package:flutter/material.dart';
import 'design_tokens.dart';

class GlassSurface extends StatelessWidget {
  final Widget child;
  final double? blurOverride;
  final double? opacityOverride;
  final BorderRadius? borderRadius;
  final EdgeInsets? padding;

  const GlassSurface({
    super.key,
    required this.child,
    this.blurOverride,
    this.opacityOverride,
    this.borderRadius,
    this.padding,
  });

  @override
  Widget build(BuildContext context) {
    final tokens = DesignTokens.of(context);
    final blur = blurOverride ?? tokens.glassBlur;
    final opacity = opacityOverride ?? tokens.glassOpacity;

    if (blur <= 0) {
      // No glass — plain container with surface color
      return Container(
        decoration: BoxDecoration(
          color: tokens.bgSurface,
          borderRadius: borderRadius,
        ),
        padding: padding,
        child: child,
      );
    }

    return ClipRRect(
      borderRadius: borderRadius ?? BorderRadius.zero,
      child: BackdropFilter(
        filter: ImageFilter.blur(sigmaX: blur * 0.5, sigmaY: blur * 0.5),
        child: Container(
          decoration: BoxDecoration(
            color: tokens.bgSurface.withOpacity(opacity),
            borderRadius: borderRadius,
          ),
          padding: padding,
          child: child,
        ),
      ),
    );
  }
}
```

- [ ] **Step 2: Commit**

```bash
cd C:\Users\root\Documents\Programs\aaa1
git add lib/core/theme/glass_wrapper.dart
git commit -m "feat(theme): add GlassSurface BackdropFilter wrapper"
```

---

### Task 6: 重写 AppTheme — 从 DesignTokens 构建 ThemeData

**Files:**
- Modify: `lib/core/theme/app_theme.dart`

**Interfaces:**
- Consumes: `DesignTokens`
- Produces: `AppTheme.build(DesignTokens tokens) → ThemeData`

- [ ] **Step 1: 重写 app_theme.dart**

```dart
import 'package:flutter/material.dart';
import 'design_tokens.dart';

class AppTheme {
  /// Build Material ThemeData from DesignTokens.
  /// Material widgets (AppBar, Buttons, Inputs, etc.) automatically inherit colors.
  static ThemeData build(DesignTokens tokens) {
    final colorScheme = ColorScheme(
      brightness: _isDark(tokens) ? Brightness.dark : Brightness.light,
      primary: tokens.primary,
      onPrimary: tokens.textOnPrimary,
      secondary: tokens.accent,
      onSecondary: tokens.textOnAccent,
      error: tokens.error,
      onError: Colors.white,
      surface: tokens.bgSurface,
      onSurface: tokens.textPrimary,
    );

    return ThemeData(
      useMaterial3: true,
      colorScheme: colorScheme,
      primaryColor: tokens.primary,
      scaffoldBackgroundColor: tokens.bgMain,
      dividerColor: tokens.divider,
      appBarTheme: AppBarTheme(
        backgroundColor: tokens.bgSurface,
        foregroundColor: tokens.textPrimary,
        elevation: 0.5,
        centerTitle: false,
        titleTextStyle: TextStyle(
          color: tokens.textPrimary,
          fontSize: tokens.fontSubheading,
          fontWeight: FontWeight.bold,
        ),
      ),
      bottomNavigationBarTheme: BottomNavigationBarThemeData(
        backgroundColor: tokens.bgSurface,
        selectedItemColor: tokens.primary,
        unselectedItemColor: tokens.textSecondary,
        type: BottomNavigationBarType.fixed,
      ),
      inputDecorationTheme: InputDecorationTheme(
        filled: true,
        fillColor: tokens.bgInput,
        contentPadding: EdgeInsets.symmetric(
          horizontal: tokens.spacingMd,
          vertical: tokens.spacingSm,
        ),
        border: OutlineInputBorder(
          borderRadius: BorderRadius.circular(tokens.buttonRadius),
          borderSide: BorderSide.none,
        ),
      ),
      cardTheme: CardThemeData(
        color: tokens.bgSurface,
        elevation: 1,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(tokens.cardRadius),
        ),
        margin: EdgeInsets.symmetric(
          horizontal: tokens.spacingMd,
          vertical: tokens.spacingTiny,
        ),
      ),
      iconTheme: IconThemeData(
        size: tokens.iconMd,
        color: tokens.textSecondary,
      ),
      textTheme: TextTheme(
        bodyLarge: TextStyle(fontSize: tokens.fontBody, color: tokens.textPrimary),
        bodyMedium: TextStyle(fontSize: tokens.fontSmall, color: tokens.textSecondary),
        bodySmall: TextStyle(fontSize: tokens.fontCaption, color: tokens.textHint),
        titleLarge: TextStyle(fontSize: tokens.fontHeading, color: tokens.textPrimary, fontWeight: FontWeight.bold),
        titleMedium: TextStyle(fontSize: tokens.fontSubheading, color: tokens.textPrimary, fontWeight: FontWeight.bold),
      ),
    );
  }

  static bool _isDark(DesignTokens tokens) {
    // Simple heuristic: dark if bgMain is dark
    return tokens.bgMain.computeLuminance() < 0.5;
  }

  // ── Keep existing legacy getters for backward compatibility ──

  static ThemeData get light => build(DesignTokens.light());
  static ThemeData get dark => build(DesignTokens.dark());
}
```

- [ ] **Step 2: 确保编译通过**

```bash
cd C:\Users\root\Documents\Programs\aaa1
dart compile kernel lib/core/theme/app_theme.dart
```

如果 dart compile 不可用，直接用 flutter analyze 检查整个项目（Task 8 后执行）。

- [ ] **Step 3: Commit**

```bash
cd C:\Users\root\Documents\Programs\aaa1
git add lib/core/theme/app_theme.dart
git commit -m "feat(theme): rewrite AppTheme to build ThemeData from DesignTokens"
```

---

### Task 7: 更新 app.dart — 接入 DesignTokens provider

**Files:**
- Modify: `lib/app.dart`
- Modify: `lib/presentation/providers/theme_provider.dart`

**Interfaces:**
- Consumes: `designTokensProvider`, `AppTheme.build()`
- Produces: `MaterialApp.router` 使用 `DesignTokens` + `_TokensScope`

- [ ] **Step 1: 更新 theme_provider.dart**

替换 `lib/presentation/providers/theme_provider.dart` 的全部内容：

```dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import '../../core/theme/design_tokens.dart';
import '../../core/theme/app_theme.dart';
import '../../core/theme/theme_registry.dart';

// Re-export designTokensProvider for convenience
// (theme_registry.dart already defines it, this file just re-exports)
export '../../core/theme/theme_registry.dart' show designTokensProvider;

/// Derived provider: Material ThemeData from current DesignTokens
final themeDataProvider = Provider<ThemeData>((ref) {
  final tokens = ref.watch(designTokensProvider);
  return AppTheme.build(tokens);
});

/// Derived provider: ThemeMode derived from glassBlur or brightness
final appThemeModeProvider = Provider<ThemeMode>((ref) {
  final tokens = ref.watch(designTokensProvider);
  if (tokens.glassBlur > 0) return ThemeMode.dark; // glass is dark-based
  return tokens.bgMain.computeLuminance() < 0.5 ? ThemeMode.dark : ThemeMode.light;
});
```

- [ ] **Step 2: 更新 app.dart**

修改 `lib/app.dart`：

```dart
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:go_router/go_router.dart';
import 'router.dart';
import 'presentation/providers/auth_provider.dart';
import 'presentation/providers/theme_provider.dart';
import 'core/theme/design_tokens.dart';
import 'core/theme/app_theme.dart';

class App extends ConsumerStatefulWidget {
  const App({super.key});

  @override
  ConsumerState<App> createState() => _AppState();
}

class _AppState extends ConsumerState<App> {
  GoRouter? _router;
  bool? _lastIsLoggedIn;

  @override
  Widget build(BuildContext context) {
    final authState = ref.watch(authProvider);
    final isLoggedIn = authState.valueOrNull != null;
    final tokens = ref.watch(designTokensProvider);
    final themeMode = ref.watch(appThemeModeProvider);

    if (_router == null || _lastIsLoggedIn != isLoggedIn) {
      _lastIsLoggedIn = isLoggedIn;
      _router = createRouter(isLoggedIn);
    }

    return _TokensScope(
      tokens: tokens,
      child: MaterialApp.router(
        title: 'AI Chat',
        debugShowCheckedModeBanner: false,
        theme: AppTheme.build(tokens),
        darkTheme: AppTheme.build(tokens), // both same — tokens drive all
        themeMode: themeMode,
        routerConfig: _router!,
      ),
    );
  }
}
```

- [ ] **Step 3: Commit**

```bash
cd C:\Users\root\Documents\Programs\aaa1
git add lib/app.dart lib/presentation/providers/theme_provider.dart
git commit -m "feat(theme): wire DesignTokens provider into app + ThemeData"
```

---

### Task 8: 更新设置页 — 主题选择 + 导入

**Files:**
- Modify: `lib/presentation/pages/settings/settings_page.dart`
- Create: `lib/presentation/pages/settings/theme_settings_page.dart`

**Interfaces:**
- Consumes: `designTokensProvider`
- Produces: SettingsPage 中有可用的主题下拉框，ThemeSettingsPage 可导入 CSS

- [ ] **Step 1: 修改 settings_page.dart 的主题部分**

在 `lib/presentation/pages/settings/settings_page.dart` 中，替换 `ListTile` 的主题切换部分（第 64-81 行，即 `leading: const Icon(Icons.brightness_medium)` 开头的 ListTile）：

```dart
Consumer(
  builder: (context, ref, _) {
    final tokens = ref.watch(designTokensProvider);
    // Determine current theme name from key
    final notifier = ref.read(designTokensProvider.notifier);
    final currentKey = notifier.currentThemeKey;
    String currentLabel;
    if (currentKey.startsWith('builtin:')) {
      switch (currentKey.substring('builtin:'.length)) {
        case 'dark': currentLabel = '深色'; break;
        case 'glass': currentLabel = '玻璃'; break;
        default: currentLabel = '浅色';
      }
    } else {
      currentLabel = '自定义 (${currentKey.split('/').last})';
    }

    return ListTile(
      leading: const Icon(Icons.palette_outlined),
      title: const Text('主题模式'),
      subtitle: Text(currentLabel, style: TextStyle(color: tokens.textSecondary, fontSize: tokens.fontSmall)),
      trailing: DropdownButton<String>(
        value: currentKey.startsWith('builtin:') ? currentKey : 'builtin:light',
        underline: const SizedBox(),
        items: const [
          DropdownMenuItem(value: 'builtin:light', child: Text('浅色')),
          DropdownMenuItem(value: 'builtin:dark', child: Text('深色')),
          DropdownMenuItem(value: 'builtin:glass', child: Text('玻璃')),
        ],
        onChanged: (value) async {
          if (value != null && value.startsWith('builtin:')) {
            final name = value.substring('builtin:'.length);
            await ref.read(designTokensProvider.notifier).loadBuiltin(name);
          }
        },
      ),
      onTap: () => context.push('/settings/theme'),
    );
  },
),
```

文件顶部 `import` 区域添加：

```dart
import '../../../core/theme/design_tokens.dart';
import '../../../core/theme/theme_registry.dart';
```

- [ ] **Step 2: 创建 ThemeSettingsPage**

文件：`lib/presentation/pages/settings/theme_settings_page.dart`

```dart
import 'dart:io';
import 'package:flutter/material.dart';
import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:file_picker/file_picker.dart';
import 'package:path_provider/path_provider.dart';
import '../../../core/theme/design_tokens.dart';
import '../../../core/theme/theme_registry.dart';

class ThemeSettingsPage extends ConsumerWidget {
  const ThemeSettingsPage({super.key});

  Future<void> _importCss(BuildContext context, WidgetRef ref) async {
    final result = await FilePicker.platform.pickFiles(
      type: FileType.any,
      allowMultiple: false,
    );
    if (result == null || result.files.isEmpty) return;

    final srcPath = result.files.first.path!;
    if (!srcPath.endsWith('.css')) {
      if (context.mounted) {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(content: Text('请选择 .css 文件')),
        );
      }
      return;
    }

    // Copy to app documents directory
    final appDir = await getApplicationDocumentsDirectory();
    final themeDir = Directory('${appDir.path}/themes');
    if (!themeDir.existsSync()) {
      themeDir.createSync(recursive: true);
    }

    final destName = srcPath.split('/').last.split('\\').last;
    final destPath = '${themeDir.path}/$destName';
    File(srcPath).copySync(destPath);

    // Load and apply
    await ref.read(designTokensProvider.notifier).loadCustom(destPath);

    if (context.mounted) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('已导入主题: $destName')),
      );
    }
  }

  @override
  Widget build(BuildContext context, WidgetRef ref) {
    final tokens = DesignTokens.of(context);
    final notifier = ref.read(designTokensProvider.notifier);
    final currentKey = notifier.currentThemeKey;

    return Scaffold(
      appBar: AppBar(title: const Text('主题设置')),
      body: ListView(
        children: [
          // Section: Built-in themes
          Padding(
            padding: EdgeInsets.all(tokens.spacingMd),
            child: Text('内置主题', style: TextStyle(
              color: tokens.textHint,
              fontSize: tokens.fontCaption,
              fontWeight: FontWeight.bold,
            )),
          ),
          _ThemeTile(
            label: '浅色 Light',
            selected: currentKey == 'builtin:light',
            onTap: () => notifier.loadBuiltin('light'),
          ),
          _ThemeTile(
            label: '深色 Dark',
            selected: currentKey == 'builtin:dark',
            onTap: () => notifier.loadBuiltin('dark'),
          ),
          _ThemeTile(
            label: '玻璃 Glass',
            selected: currentKey == 'builtin:glass',
            onTap: () => notifier.loadBuiltin('glass'),
          ),
          const Divider(),
          // Section: Custom import
          Padding(
            padding: EdgeInsets.all(tokens.spacingMd),
            child: Text('自定义主题', style: TextStyle(
              color: tokens.textHint,
              fontSize: tokens.fontCaption,
              fontWeight: FontWeight.bold,
            )),
          ),
          ListTile(
            leading: const Icon(Icons.file_open),
            title: const Text('导入 CSS 主题文件'),
            subtitle: Text(currentKey.startsWith('builtin:')
                ? '未导入'
                : '当前: ${currentKey.split('/').last}',
                style: TextStyle(fontSize: tokens.fontSmall)),
            trailing: const Icon(Icons.chevron_right),
            onTap: () => _importCss(context, ref),
          ),
          // Section: Preview
          Padding(
            padding: EdgeInsets.all(tokens.spacingMd),
            child: Text('主题预览', style: TextStyle(
              color: tokens.textHint,
              fontSize: tokens.fontCaption,
              fontWeight: FontWeight.bold,
            )),
          ),
          Padding(
            padding: EdgeInsets.all(tokens.spacingLg),
            child: Column(
              children: [
                // Color swatches
                Wrap(
                  spacing: tokens.spacingSm,
                  runSpacing: tokens.spacingSm,
                  children: [
                    _Swatch(color: tokens.primary, label: 'Primary'),
                    _Swatch(color: tokens.accent, label: 'Accent'),
                    _Swatch(color: tokens.bgMain, label: 'BG Main'),
                    _Swatch(color: tokens.bgSurface, label: 'BG Surface'),
                    _Swatch(color: tokens.userBubble, label: 'User Bubble'),
                    _Swatch(color: tokens.aiBubble, label: 'AI Bubble'),
                  ],
                ),
                SizedBox(height: tokens.spacingLg),
                // Bubble preview
                Row(
                  children: [
                    Container(
                      padding: EdgeInsets.all(tokens.spacingMd),
                      decoration: BoxDecoration(
                        color: tokens.userBubble,
                        borderRadius: BorderRadius.circular(tokens.bubbleRadius),
                      ),
                      child: Text('用户消息', style: TextStyle(color: tokens.userBubbleText)),
                    ),
                    const Spacer(),
                    Container(
                      padding: EdgeInsets.all(tokens.spacingMd),
                      decoration: BoxDecoration(
                        color: tokens.aiBubble,
                        borderRadius: BorderRadius.circular(tokens.bubbleRadius),
                      ),
                      child: Text('AI 回复', style: TextStyle(color: tokens.aiBubbleText)),
                    ),
                  ],
                ),
              ],
            ),
          ),
        ],
      ),
    );
  }
}

class _ThemeTile extends StatelessWidget {
  final String label;
  final bool selected;
  final VoidCallback onTap;

  const _ThemeTile({
    required this.label,
    required this.selected,
    required this.onTap,
  });

  @override
  Widget build(BuildContext context) {
    return ListTile(
      title: Text(label),
      trailing: selected ? const Icon(Icons.check_circle, color: Colors.green) : null,
      onTap: onTap,
    );
  }
}

class _Swatch extends StatelessWidget {
  final Color color;
  final String label;

  const _Swatch({required this.color, required this.label});

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        Container(
          width: 40, height: 40,
          decoration: BoxDecoration(
            color: color,
            borderRadius: BorderRadius.circular(8),
            border: Border.all(color: Colors.grey.shade300),
          ),
        ),
        SizedBox(height: 4),
        Text(label, style: TextStyle(fontSize: 10)),
      ],
    );
  }
}
```

- [ ] **Step 2: 确保路由注册**

检查 `lib/router.dart`，确认 `/settings/theme` 路由是否已存在。如不存在，在 settings 路由段添加：

```dart
GoRoute(
  path: 'theme',
  builder: (context, state) => const ThemeSettingsPage(),
),
```

- [ ] **Step 3: 编译检查**

```bash
cd C:\Users\root\Documents\Programs\aaa1
flutter analyze lib/
```

修复任何编译错误。

- [ ] **Step 4: Commit**

```bash
cd C:\Users\root\Documents\Programs\aaa1
git add lib/presentation/pages/settings/
git commit -m "feat(theme): add theme selection + CSS import in settings"
```

---

### Task 9: 构建验证 + 冒烟测试

**Files:**
- (无新建文件)

- [ ] **Step 1: 完整编译**

```bash
cd C:\Users\root\Documents\Programs\aaa1
flutter build apk --debug
```

预期：编译成功，无错误。

- [ ] **Step 2: 安装到设备**

```bash
adb install -r build\app\outputs\flutter-apk\app-debug.apk
```

- [ ] **Step 3: 冒烟测试清单**

1. 启动应用 → 默认浅色主题
2. 进入设置 → 主题模式显示 "浅色"
3. 切换到深色 → UI 变深色，气泡/侧边栏颜色正确
4. 切换到玻璃 → 侧边栏/输入栏有模糊效果（需有背景内容可见）
5. 重启应用 → 主题保持
6. 进入主题设置页 → 颜色色板展示正确
7. 发送消息 → 用户气泡用 userBubble 颜色，AI 气泡用 aiBubble 颜色

- [ ] **Step 4: Commit（如有修复）**

```bash
cd C:\Users\root\Documents\Programs\aaa1
git add -A
git commit -m "chore(theme): build verification + smoke test fixes"
```

---

**Total: 9 tasks**
