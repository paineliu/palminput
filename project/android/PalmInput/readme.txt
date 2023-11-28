1、手动下载 gradle
Android Studio Gradle自动下载巨慢，因此手动配置
下载好的压缩包和解压后的文件夹复制到C:/Users/用户名/.gradle/wrapper/dists
gradle-X.XX.X.bin —>8bnwg5hd3w55iofp58khbp6yv文件夹下；

将gradle-2.14.1-all.zip.part文件删除；
复制一份gradle-2.14.1-all.zip.lck文件，重命名为gradle-2.14.1-all.zip.ok；

重启as。
下载地址：https://services.gradle.org/distributions/

2、在AndroidManifest中声明 IME 组件

在 Android 系统中，IME 是包含一项特殊 IME 服务的 Android 应用。
应用的清单文件必须声明该服务，请求必需的权限，提供一个与操作 action.view.InputMethod 匹配的 Intent 过滤器，
并提供定义 IME 特征的元数据。此外，要提供可让用户修改 IME 行为的设置界面，您可以定义一个可通过系统设置启动的“设置”Activity。

以下代码段声明了一项 IME 服务。它请求了 BIND_INPUT_METHOD 权限以让该服务可以将 IME 连接到系统，设置了一个与 android.view.InputMethod 操作匹配的 Intent 过滤器，并定义了 IME 的元数据：

<!-- Declares the input method service -->
<service android:name="FastInputIME"
    android:label="@string/fast_input_label"
    android:permission="android.permission.BIND_INPUT_METHOD">
    <intent-filter>
        <action android:name="android.view.InputMethod" />
    </intent-filter>
    <meta-data android:name="android.view.im"
               android:resource="@xml/method" />
</service>
