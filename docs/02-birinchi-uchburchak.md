# 02. Birinchi uchburchak: shader, VBO va VAO

> 2026-11-13 da bu dars qadam-baqadam qo'llanma ko'rinishiga o'tkazildi: har bir qadamda avval nazariya, keyin shu qadamning
> kodi va uni qayerga yozish, oxirida yig'ib tekshirish. **Yakuniy kod o'zgarmagan.** 02-darsni bajarib bo'lgansiz, loyihangizda
> hech narsani almashtirish shart emas. Har bir oraliq qadamning kodi alohida yig'ilib, ishga tushirib tekshirildi.

## Bu darsda nima qo'shamiz va nega aynan hozir

01-darsda GPU'ga faqat "hammasini bir rangga bo'ya" dedik. Endi unga o'z dasturimizni (**shader**) va o'z ma'lumotimizni (uchta nuqta)
beramiz, u esa rangli uchburchak chizadi.

Keyingi hamma chizish shu mexanizmga tayanadi. 03-darsda bitta zarracha ikkita uchburchakdan yasaladi, minglab zarracha esa ularning
nusxalari bo'ladi. III qismda simulyatsiyaning o'zi ham shader'lar ichiga ko'chadi.

Oldindan kompyuter grafikasi yoki matematika bilish shart emas. Har bir tushuncha noldan tushuntiriladi: avval kundalik misol, keyin
kichik sonlar, oxirida kodda qayerda turgani. Dars oxirida qisqa lug'at bor.

Dars oxirida loyiha shunday bo'ladi:

```
opengl_fluid_sim_scratch/
├── CMakeLists.txt          (o'zgaradi)
├── shaders/                (yangi)
│   ├── triangle.vert
│   └── triangle.frag
└── src/
    ├── gl.hpp              (o'zgaradi)
    ├── shader.hpp          (yangi)
    ├── shader.cpp          (yangi)
    └── main.cpp            (o'zgaradi)
```

## Dars qanday tuzilgan

Dars 4 qadamdan iborat. Har bir qadamda: **nazariya**, keyin shu qadamning **kodi** va uni qayerga yozish, keyin **yig'ib ko'ring**.
Har bir qadamdan keyin dastur yig'iladi va ishlaydi.

| Qadam | Nima qilamiz | Natija |
|---|---|---|
| 1 | Fonni doimiy rangga o'tkazamiz | oyna to'q, deyarli qora ko'k |
| 2 | Birinchi shader'larni yozamiz | to'q sariq uchburchak |
| 3 | Uchlarni GPU xotirasiga joylaymiz: VBO va VAO | xuddi shu uchburchak, endi uchlari C++ dan keladi |
| 4 | Har bir uchga rang beramiz | rangli, silliq o'tadigan uchburchak |

Kod 01-darsdagidek ko'rsatiladi. Yangi fayl to'liq beriladi. Mavjud faylga kiritiladigan o'zgarish **diff** ko'rinishida beriladi:
`-` bilan boshlangan qatorni o'chiring, `+` bilan boshlanganini qo'shing. Belgisiz qatorlar joyni topish uchun, ularga tegmang.
`+` va `-` belgisining o'zini yozmang. Har bir qadam oxirida o'zgargan faylning to'liq holati "▶" belgili yashirin blokda turadi.

Yig'ish va ishga tushirish har qadamda bir xil:

```
cmake --build build
.\build\fluid_sim.exe      # Windows (PowerShell)
./build/fluid_sim          # macOS
```

**Boshlanish nuqtasi** 01-darsning oxiridagi holat: oyna rangi vaqt bilan o'zgaradi, sarlavhada FPS va o'lchamlar.

## 1-qadam. Doimiy fon

01-darsda rang ataylab o'zgarib turardi: dastur qotib qolmaganini ko'rish uchun. Endi oyna ustida narsa chizamiz, fon esa xalaqit
bermasligi kerak. Shuning uchun uni deyarli qora to'q ko'k rangga o'tkazamiz. Bu rang kurs oxirigacha qoladi.

### 1.1. Kod: `src/main.cpp`

`<cmath>` endi kerak emas: `sin` ishlatilmaydi.

<!-- diff t0 t1 src/main.cpp -->
```diff
 #include <GLFW/glfw3.h>
 
-#include <cmath>
 #include <cstdio>
```

Sikl ichida o'zgaruvchan rang o'rniga doimiy rang:

<!-- diff t0 t1 src/main.cpp -->
```diff
         glViewport(0, 0, fb_width, fb_height);
 
-        const double now = glfwGetTime();
-        const float wave = 0.5f + 0.5f * static_cast<float>(std::sin(now));
-        glClearColor(0.05f, 0.15f + 0.25f * wave, 0.35f + 0.45f * wave, 1.0f);
+        glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT);
```

Vaqt endi faqat FPS uchun kerak, shuning uchun `now` ni o'sha yerga ko'chiramiz:

<!-- diff t0 t1 src/main.cpp -->
```diff
         fps_frames++;
+        const double now = glfwGetTime();
         if (now - fps_time >= 0.5) {
```

### 1.2. Yig'ib ko'ring

Oyna deyarli qora to'q ko'k bo'ladi va o'zgarmaydi. Markazdagi pikselni o'lchadim: `(13, 18, 31)`. Qo'lda: `0.05 × 255 = 12.75`, ya'ni 13;
`0.07 × 255 = 17.85`, ya'ni 18; `0.12 × 255 = 30.6`, ya'ni 31. Sarlavhada avvalgidek `165 FPS`.

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/main.cpp</code> (solishtirish uchun)</summary>

```cpp
#include "gl.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdio>

static void on_glfw_error(int code, const char* description) {
    std::fprintf(stderr, "GLFW xatosi %d: %s\n", code, description);
}

static const char* gl_string(GLenum name) {
    return reinterpret_cast<const char*>(glGetString(name));
}

int main() {
    glfwSetErrorCallback(on_glfw_error);

    if (!glfwInit()) {
        return 1;
    }
    std::printf("GLFW:   %s\n", glfwGetVersionString());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluid Sim", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gl_load()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::printf("OpenGL: %s\n", gl_string(GL_VERSION));
    std::printf("GLSL:   %s\n", gl_string(GL_SHADING_LANGUAGE_VERSION));
    std::printf("GPU:    %s\n", gl_string(GL_RENDERER));

    glfwSwapInterval(1);

    double fps_time = glfwGetTime();
    int fps_frames = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int fb_width = 0;
        int fb_height = 0;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        glViewport(0, 0, fb_width, fb_height);

        glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        gl_check("kadr");

        glfwSwapBuffers(window);

        fps_frames++;
        const double now = glfwGetTime();
        if (now - fps_time >= 0.5) {
            int win_width = 0;
            int win_height = 0;
            glfwGetWindowSize(window, &win_width, &win_height);

            char title[128];
            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS | oyna %dx%d | framebuffer %dx%d",
                          fps_frames / (now - fps_time), win_width, win_height, fb_width, fb_height);
            glfwSetWindowTitle(window, title);

            fps_time = now;
            fps_frames = 0;
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

</details>

## 2-qadam. Birinchi shader'lar: uchburchak

Bu qadamda GPU'ga o'z dasturimizni beramiz va birinchi uchburchakni chizamiz. Uchburchak uchlarining o'rni hozircha shader'ning o'zida
yozilgan bo'ladi, ranggi esa bitta. Ma'lumotni C++ dan yuborishni 3-qadamda o'rganamiz.

Bu qadamda oltita faylga tegamiz, shuning uchun dasturni oxirida, 2.11-bo'limda yig'amiz.

### 2.1. GPU qanday chizadi: konveyer

01-darsdan eslang: GPU minglab oddiy ishchidan iborat. Ularga "mana bu pikselni shunday bo'ya" deb bittalab aytmaymiz. Ikki narsa beramiz:
**ma'lumot** (uchburchak uchlari) va **dastur** (ular bilan nima qilish kerak). GPU ma'lumotni zavod konveyeridek bosqichma-bosqich o'tkazadi.
Bu konveyer **pipeline** deyiladi:

```
uchlar haqidagi ma'lumot
     │
     ▼
Vertex shader        ← biz yozamiz, har bir uch uchun bir marta ishlaydi
     │
     ▼
Uchburchak yig'ish va kesish (clipping)
     │
     ▼
Rasterizer           ← uchburchak qaysi piksellarni qoplashini topadi
     │
     ▼
Fragment shader      ← biz yozamiz, har bir qoplangan piksel uchun bir marta ishlaydi
     │
     ▼
Framebuffer (01-dars)
```

**Shader** GPU'da ishlaydigan kichik dastur. Biz ikkitasini yozamiz.

**Vertex** bu uchburchakning bitta uchi va unga tegishli ma'lumotlar: o'rni, keyinroq rangi ham. Bizda 3 ta vertex, demak vertex shader
3 marta ishlaydi.

**Fragment** bu "piksel bo'lishga nomzod". Bizning uchburchak ekranda 152 064 ta pikselni qoplaydi (o'lchadim), demak fragment shader har
kadrda shuncha marta ishlaydi. Soniyasiga 165 kadr bo'lsa, bu soniyasiga taxminan 25 million marta. Mana shuning uchun bu ishni minglab GPU
ishchilari bajaradi.

**Nega aynan uchburchak?** Uch nuqta orqali har doim bitta tekis yuza o'tadi, to'rtta nuqta esa egilgan bo'lishi mumkin. Uchburchakda "botiq"
joy ham bo'lmaydi, shuning uchun qaysi piksel ichkarida ekanini hisoblash sodda va tez. Murakkab shakllar esa uchburchaklarga bo'linadi:
to'rtburchak ikkita uchburchak (03-dars), doira ko'p uchburchak.

### 2.2. Koordinatalar: NDC

GPU bizning birliklarimizni bilmaydi. Vertex shader har bir uchning o'rnini `gl_Position` ga yozadi, GPU esa uni shu kvadrat bo'yicha
joylaydi. Bu koordinatalar **NDC** (Normalized Device Coordinates, "normallashtirilgan qurilma koordinatalari") deyiladi:

```
            y = +1
              ▲
              │
x = -1 ───────┼───────► x = +1
              │
            y = -1
```

- Markaz `(0, 0)`, `x` o'ngga, `y` yuqoriga o'sadi. Kvadratning chetlari doim `-1` va `+1`.
- Kvadrat butun oynani qoplaydi, oyna qanday shaklda bo'lmasin.
- Kvadratdan tashqaridagi qism kesib tashlanadi (**clipping**, "kesish").

**NDC'dan piksellarga.** 1280 × 720 oynada `x = -1` chap chet (0-piksel), `x = +1` o'ng chet (1280-piksel). Demak markazdan chetgacha 640 piksel.
`y` da esa 360 piksel, va piksellar odatda yuqoridan pastga sanaladi, shuning uchun `y` ning ishorasi almashadi:

```
piksel_x = 640 + x × 640
piksel_y = 360 - y × 360
```

Bizning uchburchak uchlari:

| uch | NDC | piksel |
|---|---|---|
| chap past | (-0.6, -0.5) | (256, 540) |
| o'ng past | (0.6, -0.5) | (1024, 540) |
| tepa | (0, 0.6) | (640, 144) |

**Cho'zilish.** `x` da bitta NDC birligi 640 piksel, `y` da esa 360 piksel. Ya'ni hamma narsa eniga `640 / 360 = 1.78` baravar cho'zilgan
bo'lib chiqadi. Bu darsda bunga chidaymiz, 03-darsda proyeksiya bilan to'g'rilaymiz.

**`gl_Position` to'rtta sondan iborat:** `x`, `y`, `z`, `w`. `z` chuqurlik (3D da qaysi narsa oldinda), `w` perspektiva uchun (15-dars). 2D da `z = 0`,
`w = 1`.

### 2.3. GLSL tili

Shader'lar **GLSL** tilida yoziladi. U C/C++ ga juda o'xshaydi: `void main()`, `float`, nuqta-vergul. Fayl `#version 410 core` qatori bilan
boshlanadi: "OpenGL 4.1 ning GLSL'i, core profil".

GLSL'da kichik **vektor** turlari bor, ular bir nechta sonni birga saqlaydi:

| tur | nechta son | misol |
|---|---|---|
| `float` | 1 | `0.5` |
| `vec2` | 2 | o'rin `(x, y)` |
| `vec3` | 3 | rang `(qizil, yashil, ko'k)` |
| `vec4` | 4 | `gl_Position`, shaffoflikli rang |

`vec4(1.0, 0.5, 0.0, 1.0)` to'rt sondan vektor yasaydi. `vec4(p, 0.0, 1.0)` esa ikki sonli `p` ga yana ikkita son qo'shib, to'rt sonli
vektor yasaydi.

`gl_` bilan boshlanadigan nomlar GLSL'ning o'zida tayyor turadi. Bizga ikkitasi kerak:

- **`gl_Position`** vertex shader yozadigan natija: uchning NDC'dagi o'rni.
- **`gl_VertexID`** vertex shader ichida "hozir nechanchi uch ustida ishlayapman": 0, 1 yoki 2.

### 2.4. Kod: shader fayllari

Loyihaning ildizida `shaders` papkasini yarating va unga ikki fayl qo'shing.

`shaders/triangle.vert`:

<!-- file t2 shaders/triangle.vert -->
```glsl
#version 410 core

const vec2 positions[3] = vec2[3](
    vec2(-0.6, -0.5),
    vec2(0.6, -0.5),
    vec2(0.0, 0.6)
);

void main() {
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
}
```

**`const vec2 positions[3]`** uchta `vec2` dan iborat massiv (ro'yxat), qiymatlari 2.2-bo'limdagi jadvaldan. `const` "o'zgarmaydi" degani.
`vec2[3](...)` massivni qiymatlar bilan to'ldiradi.

**`positions[gl_VertexID]`** massivdan hozirgi uchning o'rnini oladi: 0-uch uchun `positions[0]`, ya'ni `(-0.6, -0.5)`, va hokazo.

**`vec4(..., 0.0, 1.0)`** ikki sonni to'rt sonli `gl_Position` ga aylantiradi: `z = 0`, `w = 1` (2.2-bo'lim).

Bu fayl vaqtinchalik: 3-qadamda uchlar C++ dan keladi va massiv olib tashlanadi. `gl_VertexID` esa 03-darsda yana kerak bo'ladi.

`shaders/triangle.frag`:

<!-- file t2 shaders/triangle.frag -->
```glsl
#version 410 core

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(1.0, 0.5, 0.0, 1.0);
}
```

**`out vec4 out_color`** fragment shader'ning natijasi: shu pikselning rangi. `out` "bu bosqichdan chiqadi" degani. **`layout(location = 0)`**
"framebuffer'ning 0-rang chiqishiga yoz" degani: bizda bitta chiqish bor, u ekrandagi rasm.

**`vec4(1.0, 0.5, 0.0, 1.0)`** to'q sariq: qizil to'liq, yashil yarim, ko'k yo'q, alpha 1 (to'liq ko'rinadi, 01-dars). Hamma piksel bir xil rangda.

### 2.5. Shader'ni kompilyatsiya va link qilish

00-darsdan eslang: C++ matni compiler va linker orqali dasturga aylanadi. Shader ham xuddi shunday, faqat bu ish **dastur ishlab turganda**,
drayver ichidagi compiler bilan bajariladi:

```
triangle.vert ──(glCompileShader)──► vertex shader obyekti ───┐
                                                              ├──(glLinkProgram)──► program
triangle.frag ──(glCompileShader)──► fragment shader obyekti ─┘
```

Shader obyekti `.obj` faylga, **program** esa `.exe` ga o'xshaydi. Link bosqichi ikkala shader bir-biriga mos kelishini tekshiradi: masalan,
ikkalasida ham `main` borligini.

**Nega oldindan kompilyatsiya qilib qo'ymaymiz?** GPU'lar har xil va har birining mashina tili boshqa. Retseptni oshxonaga matn holida beramiz,
har bir oshxonaning oshpazi uni o'z tiliga o'giradi. Shuning uchun shader matni dastur bilan birga tarqatiladi.

**Xatoni o'zimiz so'rashimiz kerak.** `glCompileShader` hech narsa qaytarmaydi va `gl_check` ham bunday xatoni ko'rmaydi. Natijani `glGetShaderiv`
bilan, xato matnini `glGetShaderInfoLog` bilan olamiz. Buni yozmasak, xato shader ekranda shunchaki bo'shliq bo'lib ko'rinadi.

**Nega shader alohida faylda.** Matnni C++ ichida yozish ham mumkin edi. Fayl qulayroq: shader o'zgarganda C++ ni qayta yig'ish shart emas, dasturni
qayta ishga tushirish yetarli (2.11-bo'limda sinab ko'rasiz). Faqat dastur fayl qayerdaligini bilishi kerak. Yo'l har kompyuterda boshqa,
shuning uchun uni CMake beradi.

### 2.6. Kod: `CMakeLists.txt`

<!-- diff t1 t2 CMakeLists.txt -->
```diff
 add_executable(fluid_sim
     src/main.cpp
     src/gl.cpp
+    src/shader.cpp
 )
 
 target_include_directories(fluid_sim PRIVATE external/khronos)
+target_compile_definitions(fluid_sim PRIVATE SHADER_DIR="${CMAKE_SOURCE_DIR}/shaders")
 target_compile_options(fluid_sim PRIVATE -Wall -Wextra)
```

`src/shader.cpp` 2.8-bo'limda yoziladigan yangi fayl. `target_compile_definitions` kodga `SHADER_DIR` degan makro beradi, xuddi
`#define SHADER_DIR "C:/.../shaders"` yozilgandek (makro 01-darsda). `${CMAKE_SOURCE_DIR}` loyihaning ildiz papkasi: CMake shu kompyuterdagi
haqiqiy yo'lni qo'yadi.

### 2.7. Kod: `src/gl.hpp`

Bu qadam uchun 17 ta yangi OpenGL funksiyasi kerak: shader va program yaratish, VAO va chizish. Ro'yxat uzunlashgani uchun `\` larni o'ngroqqa
suramiz, shunda keyingi darslardagi uzun nomlar ham sig'adi. Shuning uchun eski besh qatorni o'chirib, ro'yxatni to'liq yangisi bilan
almashtiring:

<!-- diff t1 t2 src/gl.hpp -->
```diff
-#define GL_FUNCTIONS(X)                  \
-    X(PFNGLGETERRORPROC, glGetError)     \
-    X(PFNGLGETSTRINGPROC, glGetString)   \
-    X(PFNGLVIEWPORTPROC, glViewport)     \
-    X(PFNGLCLEARCOLORPROC, glClearColor) \
-    X(PFNGLCLEARPROC, glClear)
+#define GL_FUNCTIONS(X)                                                \
+    X(PFNGLGETERRORPROC, glGetError)                                   \
+    X(PFNGLGETSTRINGPROC, glGetString)                                 \
+    X(PFNGLVIEWPORTPROC, glViewport)                                   \
+    X(PFNGLCLEARCOLORPROC, glClearColor)                               \
+    X(PFNGLCLEARPROC, glClear)                                         \
+    X(PFNGLCREATESHADERPROC, glCreateShader)                           \
+    X(PFNGLSHADERSOURCEPROC, glShaderSource)                           \
+    X(PFNGLCOMPILESHADERPROC, glCompileShader)                         \
+    X(PFNGLGETSHADERIVPROC, glGetShaderiv)                             \
+    X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)                   \
+    X(PFNGLDELETESHADERPROC, glDeleteShader)                           \
+    X(PFNGLCREATEPROGRAMPROC, glCreateProgram)                         \
+    X(PFNGLATTACHSHADERPROC, glAttachShader)                           \
+    X(PFNGLLINKPROGRAMPROC, glLinkProgram)                             \
+    X(PFNGLGETPROGRAMIVPROC, glGetProgramiv)                           \
+    X(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)                 \
+    X(PFNGLDELETEPROGRAMPROC, glDeleteProgram)                         \
+    X(PFNGLUSEPROGRAMPROC, glUseProgram)                               \
+    X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)                     \
+    X(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)               \
+    X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)                     \
+    X(PFNGLDRAWARRAYSPROC, glDrawArrays)
```

Hammasi OpenGL 4.1 da bor. Oxirgi qatorda `\` yo'q (01-dars, 3-qadam). `gl.cpp` ga tegmaymiz: X-macro yangi funksiyalarni o'zi yuklaydi.

### 2.8. Kod: `src/shader.hpp` va `src/shader.cpp` (yangi fayllar)

Shader'ni yuklash ishi har darsda kerak bo'ladi, shuning uchun uni alohida faylga chiqaramiz.

`src/shader.hpp`:

<!-- file t2 src/shader.hpp -->
```cpp
#pragma once

#include "gl.hpp"

GLuint create_program(const char* vertex_file, const char* fragment_file);
```

Bitta funksiya: ikki shader faylini o'qiydi, kompilyatsiya va link qiladi va tayyor program'ning nomini (sonni) qaytaradi. Xato bo'lsa 0
qaytaradi. `GLuint` OpenGL'ning ishorasiz butun soni: OpenGL obyektlarni shunday sonlar bilan ataydi.

`src/shader.cpp`:

<!-- file t2 src/shader.cpp -->
```cpp
#include "shader.hpp"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

static GLuint compile_shader(GLenum stage, const char* file) {
    const std::string path = std::string(SHADER_DIR) + "/" + file;
    std::ifstream stream(path);
    if (!stream) {
        std::fprintf(stderr, "Shader fayli ochilmadi: %s\n", path.c_str());
        return 0;
    }
    std::stringstream buffer;
    buffer << stream.rdbuf();
    const std::string source = buffer.str();
    const char* text = source.c_str();

    GLuint shader = glCreateShader(stage);
    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);

    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::fprintf(stderr, "%s kompilyatsiya xatosi:\n%s\n", file, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint create_program(const char* vertex_file, const char* fragment_file) {
    GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertex_file);
    GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragment_file);
    if (!vertex || !fragment) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint ok = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::fprintf(stderr, "Link xatosi (%s, %s):\n%s\n", vertex_file, fragment_file, log);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}
```

**Fayl yo'li.** `std::string` C++ ning matn turi, `+` matnlarni ulaydi: `"C:/.../shaders" + "/" + "triangle.vert"`.

**Faylni o'qish.** `std::ifstream` faylni ochadi. `if (!stream)` fayl ochilmaganini tekshiradi (masalan, nom xato yozilgan). `buffer << stream.rdbuf()`
butun faylni bitta harakatda `stringstream` ga quyadi, `buffer.str()` esa uni `std::string` qilib beradi.

**`c_str()`** C++ matnidan C uslubidagi matnni oladi: oxiri nol belgi bilan tugaydigan belgilar ketma-ketligi. OpenGL C tilida yozilgan, u faqat
shuni tushunadi.

**`glShaderSource(shader, 1, &text, nullptr)`.** Bu funksiya bir nechta matn bo'lagini birga qabul qila oladi, shuning uchun matnlar ro'yxatining
manzilini so'raydi. Bizda bitta matn: `1` "bitta bo'lak", `&text` "o'sha bitta matn ko'rsatkichining manzili". `nullptr` "matn uzunligini o'zing
hisobla, u nol belgi bilan tugaydi".

**`glGetShaderiv`** nomidagi `iv` "natija butun son (integer), u vektor (ko'rsatkich) orqali qaytadi" degani. Ko'p OpenGL nomlari shunday tuzilgan:
`glUniform2f` dagi `2f` "ikkita float" (03-dars).

**`char log[2048]`** 2048 belgilik joy. Drayver xato matnini shu yerga yozadi, `sizeof(log)` esa joydan oshib ketmaslikni aytadi.

**`static`** `compile_shader` faqat shu fayl ichida ko'rinadi, tashqariga faqat `create_program` chiqadi.

**`create_program`** ikkala shader'ni ham kompilyatsiya qiladi, shuning uchun ikkalasidagi xatolar birdan ko'rinadi. `glDeleteShader(0)` OpenGL'da
jimgina e'tiborsiz qoldiriladi, shuning uchun qaysi biri muvaffaqiyatsiz bo'lganini tekshirmasdan ikkalasini o'chiramiz.

**Link'dan keyingi `glDeleteShader`.** Program shader kodini o'zida saqlab qoldi, oraliq obyektlar endi kerak emas, xuddi `.exe` tayyor bo'lgach
`.obj` kerak bo'lmagandek.

### 2.9. Chizish uchun yana nima kerak: program va VAO

Chizish buyrug'i **`glDrawArrays(GL_TRIANGLES, 0, 3)`**: "0-uchdan boshlab 3 ta uchni uchburchak qilib chiz". Undan oldin ikki narsa
o'rnatilgan bo'lishi kerak (01-darsdagi holat mashinasi: chizish buyrug'i "hozir nima o'rnatilgan bo'lsa, shuni ishlatadi"):

- **`glUseProgram(program)`**: "shu program bilan chiz".
- **`glBindVertexArray(vao)`**: **VAO** (Vertex Array Object) uchlar ma'lumoti qayerdan va qanday o'qilishini eslab qoladigan obyekt.
  Hozircha uchlar shader ichida, VAO bo'sh. Lekin core profilda VAO'siz chizib bo'lmaydi, shuning uchun bo'sh bo'lsa ham kerak. 3-qadamda
  uni to'ldiramiz.

`glGenVertexArrays(1, &vao)` yangi VAO uchun **nom** beradi. Nom oddiy son (1, 2, 3, ...), u bilan keyin VAO'ga murojaat qilamiz. Birinchi
parametr "nechta", `&vao` natija yoziladigan joyning manzili (01-dars).

### 2.10. Kod: `src/main.cpp`

**Faylning boshida**, `gl.hpp` dan keyin:

<!-- diff t1 t2 src/main.cpp -->
```diff
 #include "gl.hpp"
+#include "shader.hpp"
 
```

**Sikldan oldin**, `glfwSwapInterval(1);` dan keyin, program va VAO'ni yarating:

<!-- diff t1 t2 src/main.cpp -->
```diff
     glfwSwapInterval(1);
 
+    GLuint program = create_program("triangle.vert", "triangle.frag");
+    if (!program) {
+        glfwDestroyWindow(window);
+        glfwTerminate();
+        return 1;
+    }
+
+    GLuint vao = 0;
+    glGenVertexArrays(1, &vao);
+    gl_check("sozlash");
+
     double fps_time = glfwGetTime();
```

`create_program` kontekst va `gl_load` dan keyin chaqiriladi, chunki u OpenGL funksiyalarini ishlatadi. Xato bo'lsa, xabar terminalga chiqqan
bo'ladi, biz esa dasturni `1` kodi bilan yopamiz. `gl_check("sozlash")` sozlash paytidagi xatolarni kadr xatolaridan ajratib ko'rsatadi.

**Sikl ichida**, `glClear` dan keyin, chizing:

<!-- diff t1 t2 src/main.cpp -->
```diff
         glClear(GL_COLOR_BUFFER_BIT);
+
+        glUseProgram(program);
+        glBindVertexArray(vao);
+        glDrawArrays(GL_TRIANGLES, 0, 3);
         gl_check("kadr");
```

`glUseProgram` va `glBindVertexArray` ni har kadrda chaqiramiz. Hozir bitta program va bitta VAO bor, bir marta yetardi. Lekin keyin bir nechtasi
bo'ladi, chizish esa har doim "hozir nima o'rnatilgan bo'lsa" shuni ishlatadi.

**Sikldan keyin**, oynani yopishdan oldin, yaratganlarimizni o'chiring:

<!-- diff t1 t2 src/main.cpp -->
```diff
     }
 
+    glDeleteVertexArrays(1, &vao);
+    glDeleteProgram(program);
+
     glfwDestroyWindow(window);
```

O'chirish `glfwDestroyWindow` dan oldin turadi, chunki OpenGL chaqiruvlari uchun kontekst tirik bo'lishi kerak.

### 2.11. Yig'ib ko'ring

To'q fonda to'q sariq uchburchak chiqadi. O'lchadim: uchburchak `x` bo'yicha 256 dan 1023 gacha, `y` bo'yicha 145 dan 539 gacha, jami 152 064 piksel.
Bu 2.2-bo'limdagi hisob bilan bir xil. Ichidagi piksellar `(255, 127, 0)`: `0.5 × 255 = 127.5`, ya'ni 127. Uchburchak eniga cho'zilgan, bu
kutilgan holat (2.2-bo'lim).

**Shader fayldan o'qilishining foydasi.** Dastur yopiq turganda `triangle.frag` dagi rangni `vec4(0.2, 0.8, 0.3, 1.0)` ga o'zgartiring va **yig'masdan**
dasturni qayta ishga tushiring. Uchburchak yashil bo'ladi: sinab ko'rganimda `(51, 204, 76)` chiqdi. Sinab bo'lgach, rangni qaytaring.

**Nima buzilsa nima bo'ladi.** Hammasini shu qadamning kodida sinab ko'rdim:

| O'zgarish | Natija |
|---|---|
| `triangle.frag` da nuqta-vergul yo'q | dastur `1` kodi bilan chiqadi, kompilyatsiya xatosi (pastda) |
| `triangle.vert` da `main` o'rniga `main2` | dastur `1` kodi bilan chiqadi, link xatosi (pastda) |
| `create_program` ga `"triangel.vert"` (xato nom) | dastur `1` kodi bilan chiqadi: `Shader fayli ochilmadi: C:/.../shaders/triangel.vert` |
| sikldagi `glBindVertexArray(vao)` o'rniga `glBindVertexArray(0)` | hech narsa chizilmaydi, har kadrda `OpenGL xatosi (kadr): GL_INVALID_OPERATION` |
| `glUseProgram` yo'q | hech narsa chizilmaydi, hech qanday xabar yo'q |

Ikki shader xatosining aniq matni:

```
# triangle.frag'da nuqta-vergul qoldirilsa:
triangle.frag kompilyatsiya xatosi:
0(7) : error C0000: syntax error, unexpected '}', expecting ',' or ';' at token "}"

# triangle.vert'da main bo'lmasa (kompilyatsiyadan o'tadi, link'da yiqiladi):
Link xatosi (triangle.vert, triangle.frag):
Vertex info
-----------
(0) : error C3001: no program defined
```

`0(7)` da `0` manba raqami, `7` qator raqami: nuqta-vergul 6-qatorda yo'q, lekin compiler buni 7-qatordagi `}` ni ko'rganda tushunadi. Mac'da
format boshqacha bo'lishi kutiladi: Khronos'ning `glslangValidator` dasturi xuddi shunday xatoni `ERROR: 0:7: ... syntax error` ko'rinishida yozadi.

**CRLF haqida.** Windows qator oxirini ikki belgi bilan, Mac bitta belgi bilan yozadi (00-dars). Shader'larni Windows uslubidagi qator oxirlari bilan
sinab ko'rdim, GLSL compiler ularni oddiy qator oxiri deb qabul qildi.

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/gl.hpp</code> (solishtirish uchun)</summary>

```cpp
#pragma once

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif
#include <GL/glcorearb.h>

#define GL_FUNCTIONS(X)                                                \
    X(PFNGLGETERRORPROC, glGetError)                                   \
    X(PFNGLGETSTRINGPROC, glGetString)                                 \
    X(PFNGLVIEWPORTPROC, glViewport)                                   \
    X(PFNGLCLEARCOLORPROC, glClearColor)                               \
    X(PFNGLCLEARPROC, glClear)                                         \
    X(PFNGLCREATESHADERPROC, glCreateShader)                           \
    X(PFNGLSHADERSOURCEPROC, glShaderSource)                           \
    X(PFNGLCOMPILESHADERPROC, glCompileShader)                         \
    X(PFNGLGETSHADERIVPROC, glGetShaderiv)                             \
    X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)                   \
    X(PFNGLDELETESHADERPROC, glDeleteShader)                           \
    X(PFNGLCREATEPROGRAMPROC, glCreateProgram)                         \
    X(PFNGLATTACHSHADERPROC, glAttachShader)                           \
    X(PFNGLLINKPROGRAMPROC, glLinkProgram)                             \
    X(PFNGLGETPROGRAMIVPROC, glGetProgramiv)                           \
    X(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)                 \
    X(PFNGLDELETEPROGRAMPROC, glDeleteProgram)                         \
    X(PFNGLUSEPROGRAMPROC, glUseProgram)                               \
    X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)                     \
    X(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)               \
    X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)                     \
    X(PFNGLDRAWARRAYSPROC, glDrawArrays)

#define X(type, name) extern type name;
GL_FUNCTIONS(X)
#undef X

bool gl_load();
void gl_check(const char* where);
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/main.cpp</code> (solishtirish uchun)</summary>

```cpp
#include "gl.hpp"
#include "shader.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdio>

static void on_glfw_error(int code, const char* description) {
    std::fprintf(stderr, "GLFW xatosi %d: %s\n", code, description);
}

static const char* gl_string(GLenum name) {
    return reinterpret_cast<const char*>(glGetString(name));
}

int main() {
    glfwSetErrorCallback(on_glfw_error);

    if (!glfwInit()) {
        return 1;
    }
    std::printf("GLFW:   %s\n", glfwGetVersionString());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluid Sim", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gl_load()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::printf("OpenGL: %s\n", gl_string(GL_VERSION));
    std::printf("GLSL:   %s\n", gl_string(GL_SHADING_LANGUAGE_VERSION));
    std::printf("GPU:    %s\n", gl_string(GL_RENDERER));

    glfwSwapInterval(1);

    GLuint program = create_program("triangle.vert", "triangle.frag");
    if (!program) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    gl_check("sozlash");

    double fps_time = glfwGetTime();
    int fps_frames = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int fb_width = 0;
        int fb_height = 0;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        glViewport(0, 0, fb_width, fb_height);

        glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        gl_check("kadr");

        glfwSwapBuffers(window);

        fps_frames++;
        const double now = glfwGetTime();
        if (now - fps_time >= 0.5) {
            int win_width = 0;
            int win_height = 0;
            glfwGetWindowSize(window, &win_width, &win_height);

            char title[128];
            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS | oyna %dx%d | framebuffer %dx%d",
                          fps_frames / (now - fps_time), win_width, win_height, fb_width, fb_height);
            glfwSetWindowTitle(window, title);

            fps_time = now;
            fps_frames = 0;
        }
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

</details>

## 3-qadam. Uchlar GPU xotirasida: VBO va VAO

Uchlar hozir shader ichida qotib qolgan. Simulyatsiyada esa minglab zarracha har kadrda joyini o'zgartiradi, ularning o'rnini C++ hisoblaydi.
Demak ma'lumot C++ dan GPU'ga borishi kerak. Bu qadamda uchlarni C++ massiviga ko'chiramiz, uni GPU xotirasiga yuboramiz va vertex shader'ga
undan o'qishni o'rgatamiz. Ekrandagi natija o'zgarmaydi, o'zgaradi faqat ma'lumot qayerdan kelishi.

### 3.1. Baytlar: `float` va `struct` xotirada

01-darsda xotirani raqamlangan uylar qatori deb tasavvur qildik, har bir uyda bitta bayt. Endi o'z ma'lumotimizni shu uylarga joylaymiz.

**`float`** (kasr son) xotirada 4 bayt egallaydi. Bizning bitta uch hozircha 2 ta `float`: `x` va `y`. Demak `2 × 4 = 8` bayt. C++ da buni
`struct` bilan yozamiz:

```cpp
struct Vertex {
    float x, y;
};
```

`struct` bir nechta qiymatni bitta nom ostida birlashtiradi. Uchta uch ketma-ket yotadi, jami `3 × 8 = 24` bayt:

```
bayt:   0    4    8    12   16   20   24
        ┌────┬────┬────┬────┬────┬────┐
        │ x  │ y  │ x  │ y  │ x  │ y  │
        └────┴────┴────┴────┴────┴────┘
        │0-uch    │1-uch    │2-uch    │
```

C++ da ikkita foydali o'lchov bor:

- **`sizeof(Vertex)`** `struct` necha bayt ekanini beradi: hozir 8.
- **`offsetof(Vertex, y)`** `y` maydoni `struct` boshidan necha baytda turishini beradi: `x` dan keyin, ya'ni 4. `offsetof(Vertex, x)` esa 0.

### 3.2. VBO: GPU xotirasidagi baytlar

**VBO** (Vertex Buffer Object) GPU xotirasidagi oddiy baytlar bloki. Unda "o'rin" yoki "rang" degan tushuncha yo'q, faqat baytlar. Bizning 24 baytimiz
shu yerga ko'chiriladi.

**Bind uslubi.** OpenGL obyektlar bilan g'alati, lekin izchil uslubda ishlaydi: avval obyektni **bog'lash nuqtasiga** (binding point) qo'yamiz, keyingi
buyruqlar o'sha nuqtadagi obyektga ta'sir qiladi. Stolda ochiq turgan daftarni tasavvur qiling: "yoz" desangiz, qaysi daftar ochiq bo'lsa, o'shanga
yoziladi.

```cpp
glBindBuffer(GL_ARRAY_BUFFER, vbo);                         // shu buffer'ni "ochiq daftar" qil
glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);  // ochiq daftarga shu baytlarni yoz
```

- `glGenBuffers(1, &vbo)` yangi buffer uchun nom beradi, xuddi `glGenVertexArrays` kabi.
- `glBufferData` ma'lumotni CPU xotirasidan GPU xotirasiga **nusxalaydi**. Shundan keyin CPU'dagi massiv kerak emas.
- `GL_STATIC_DRAW` drayverga maslahat: "bir marta yozaman, ko'p marta chizaman". 04-darsda zarrachalar har kadrda o'zgaradi va `GL_STREAM_DRAW` ga o'tamiz.

### 3.3. VAO: baytlarni qanday o'qish kerak

VBO sarlavhasiz jadvalga o'xshaydi: sonlar bor, lekin qaysi ustun nima ekani yozilmagan. 2-qadamda bo'sh qolgan **VAO** shu jadvalning sarlavhalari:
qaysi **attribute** qaysi buffer'dan, qayerdan boshlab, qanday turda o'qiladi.

**Attribute** vertex shader'ning buffer'dan keladigan kirishi. GLSL'da u `in` so'zi bilan yoziladi ("bu bosqichga kiradi"):

```glsl
layout(location = 0) in vec2 in_position;
```

`layout(location = 0)` attribute'ning raqami. C++ tomonda shu raqam bilan unga murojaat qilamiz. Vertex shader har bir uch uchun ishlaganda
`in_position` ga o'sha uchning `x` va `y` si avtomatik yoziladi.

Tavsifni `glVertexAttribPointer` beradi:

| Parametr | qiymat | Ma'nosi |
|---|---|---|
| `index` | 0 | shader'dagi `layout(location = 0)` |
| `size` | 2 | nechta son (`vec2`) |
| `type` | `GL_FLOAT` | sonlar turi |
| `normalized` | `GL_FALSE` | butun sonlarni 0..1 ga o'tkazish (float uchun kerak emas) |
| `stride` | `sizeof(Vertex)`, ya'ni 8 | bir uchning boshidan keyingisining boshigacha necha bayt (qadam) |
| `pointer` | `offsetof(Vertex, x)`, ya'ni 0 | uch boshidan necha bayt keyin turadi (siljish) |

GPU `n`-uchning o'rnini shunday topadi: `n × stride + pointer`. Masalan, 2-uch `2 × 8 + 0 = 16`-baytdan boshlanadi. 3.1-bo'limdagi rasmga qarang:
aynan shunday.

Attribute'ni yana **yoqish** kerak: `glEnableVertexAttribArray(0)`. Yoqilmagan attribute buffer'dan o'qilmaydi.

**Nozik joy.** VAO buffer'ni `glVertexAttribPointer` chaqirilgan **paytda** eslab qoladi: shu paytda `GL_ARRAY_BUFFER` ga qaysi buffer bog'langan bo'lsa,
attribute o'sha buffer'dan o'qiydi. Shuning uchun tartib: VAO, VBO, keyin `glVertexAttribPointer`. 04-darsda ikkita buffer bilan bu qoida juda kerak bo'ladi.

### 3.4. Kod: `src/gl.hpp`

Buffer bilan ishlaydigan to'rtta funksiya `glUseProgram` dan keyin:

<!-- diff t2 t3 src/gl.hpp -->
```diff
     X(PFNGLUSEPROGRAMPROC, glUseProgram)                               \
+    X(PFNGLGENBUFFERSPROC, glGenBuffers)                               \
+    X(PFNGLDELETEBUFFERSPROC, glDeleteBuffers)                         \
+    X(PFNGLBINDBUFFERPROC, glBindBuffer)                               \
+    X(PFNGLBUFFERDATAPROC, glBufferData)                               \
     X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)                     \
```

Attribute bilan ishlaydigan ikkitasi `glBindVertexArray` dan keyin:

<!-- diff t2 t3 src/gl.hpp -->
```diff
     X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)                     \
+    X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)     \
+    X(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)             \
     X(PFNGLDRAWARRAYSPROC, glDrawArrays)
```

### 3.5. Kod: `shaders/triangle.vert`

Massivni o'chirib, o'rniga attribute yozing:

<!-- diff t2 t3 shaders/triangle.vert -->
```diff
 #version 410 core
 
-const vec2 positions[3] = vec2[3](
-    vec2(-0.6, -0.5),
-    vec2(0.6, -0.5),
-    vec2(0.0, 0.6)
-);
+layout(location = 0) in vec2 in_position;
 
 void main() {
-    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
+    gl_Position = vec4(in_position, 0.0, 1.0);
 }
```

Nomdagi `in_` "buffer'dan keldi" degani. Bu GLSL qoidasi emas, faqat bizning kelishuv: kod o'qilishi oson bo'ladi.

### 3.6. Kod: `src/main.cpp`

**Include'lar ostida** `<cstddef>` (`offsetof` shu yerda) va `Vertex` tuzilmasini qo'shing:

<!-- diff t2 t3 src/main.cpp -->
```diff
 #include <GLFW/glfw3.h>
 
+#include <cstddef>
 #include <cstdio>
 
+struct Vertex {
+    float x, y;
+};
+
 static void on_glfw_error(int code, const char* description) {
```

**Program yaratilgandan keyin** uchlar massivi, VBO va attribute'ni sozlang:

<!-- diff t2 t3 src/main.cpp -->
```diff
     }
 
+    const Vertex vertices[] = {
+        {-0.6f, -0.5f},
+        {0.6f, -0.5f},
+        {0.0f, 0.6f},
+    };
+
     GLuint vao = 0;
+    GLuint vbo = 0;
     glGenVertexArrays(1, &vao);
+    glGenBuffers(1, &vbo);
+
+    glBindVertexArray(vao);
+    glBindBuffer(GL_ARRAY_BUFFER, vbo);
+    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
+
+    glEnableVertexAttribArray(0);
+    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
+                          reinterpret_cast<const void*>(offsetof(Vertex, x)));
     gl_check("sozlash");
```

Qatorma-qator:

- **`const Vertex vertices[]`** uchta uchning massivi. Har bir `{...}` bitta `Vertex`: `x`, `y`. Sonlar shader'dagi massivdan ko'chdi.
- **`glBindVertexArray(vao)`** birinchi turadi: keyingi attribute sozlamalari shu VAO'ga yoziladi.
- **`glBufferData(..., sizeof(vertices), vertices, ...)`** 24 baytni GPU'ga ko'chiradi. `sizeof(vertices)` butun massivning o'lchami: `3 × 8 = 24`.
- **`glVertexAttribPointer`** 3.3-bo'limdagi jadval. Oxirgi parametrning turi tarixiy sabab bilan ko'rsatkich: juda qadimda u CPU xotirasiga
  ko'rsatgan. Endi u buffer ichidagi bayt siljishi, shuning uchun sonni `reinterpret_cast<const void*>` bilan ko'rsatkich turiga o'tkazamiz.

`stride` uchun `8` emas, `sizeof(Vertex)` yozdik. 4-qadamda `Vertex` ga rang qo'shilganda u o'zi 20 bo'ladi va bu qatorga tegish shart bo'lmaydi.

**Sikldan keyin** buffer'ni ham o'chiring:

<!-- diff t2 t3 src/main.cpp -->
```diff
     }
 
+    glDeleteBuffers(1, &vbo);
     glDeleteVertexArrays(1, &vao);
```

### 3.7. Yig'ib ko'ring

Ekranda 2-qadamdagi uchburchakning aynan o'zi: 256..1023, 145..539, 152 064 piksel, rang `(255, 127, 0)`. Endi uning uchlari C++ dan kelayotganini
tekshirish uchun `vertices` dagi tepa uchni `{0.0f, 0.6f}` dan `{0.0f, 0.9f}` ga o'zgartirib yig'ing. Hisob: `360 - 0.9 × 360 = 36`. Sinab
ko'rganimda uchburchakning tepasi 37-pikselga ko'tarildi. Sinab bo'lgach, `0.6f` ni qaytaring.

Sinab ko'rdim: `glEnableVertexAttribArray(0);` qatori bo'lmasa, hech narsa chizilmaydi va hech qanday xabar chiqmaydi. Yoqilmagan attribute
buffer'dan o'qilmaydi, uchala uch bir nuqtaga tushadi va uchburchak yuzasiz bo'lib qoladi.

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/gl.hpp</code> (solishtirish uchun)</summary>

```cpp
#pragma once

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif
#include <GL/glcorearb.h>

#define GL_FUNCTIONS(X)                                                \
    X(PFNGLGETERRORPROC, glGetError)                                   \
    X(PFNGLGETSTRINGPROC, glGetString)                                 \
    X(PFNGLVIEWPORTPROC, glViewport)                                   \
    X(PFNGLCLEARCOLORPROC, glClearColor)                               \
    X(PFNGLCLEARPROC, glClear)                                         \
    X(PFNGLCREATESHADERPROC, glCreateShader)                           \
    X(PFNGLSHADERSOURCEPROC, glShaderSource)                           \
    X(PFNGLCOMPILESHADERPROC, glCompileShader)                         \
    X(PFNGLGETSHADERIVPROC, glGetShaderiv)                             \
    X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)                   \
    X(PFNGLDELETESHADERPROC, glDeleteShader)                           \
    X(PFNGLCREATEPROGRAMPROC, glCreateProgram)                         \
    X(PFNGLATTACHSHADERPROC, glAttachShader)                           \
    X(PFNGLLINKPROGRAMPROC, glLinkProgram)                             \
    X(PFNGLGETPROGRAMIVPROC, glGetProgramiv)                           \
    X(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)                 \
    X(PFNGLDELETEPROGRAMPROC, glDeleteProgram)                         \
    X(PFNGLUSEPROGRAMPROC, glUseProgram)                               \
    X(PFNGLGENBUFFERSPROC, glGenBuffers)                               \
    X(PFNGLDELETEBUFFERSPROC, glDeleteBuffers)                         \
    X(PFNGLBINDBUFFERPROC, glBindBuffer)                               \
    X(PFNGLBUFFERDATAPROC, glBufferData)                               \
    X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)                     \
    X(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)               \
    X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)                     \
    X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)     \
    X(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)             \
    X(PFNGLDRAWARRAYSPROC, glDrawArrays)

#define X(type, name) extern type name;
GL_FUNCTIONS(X)
#undef X

bool gl_load();
void gl_check(const char* where);
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>shaders/triangle.vert</code> (solishtirish uchun)</summary>

```glsl
#version 410 core

layout(location = 0) in vec2 in_position;

void main() {
    gl_Position = vec4(in_position, 0.0, 1.0);
}
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/main.cpp</code> (solishtirish uchun)</summary>

```cpp
#include "gl.hpp"
#include "shader.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstddef>
#include <cstdio>

struct Vertex {
    float x, y;
};

static void on_glfw_error(int code, const char* description) {
    std::fprintf(stderr, "GLFW xatosi %d: %s\n", code, description);
}

static const char* gl_string(GLenum name) {
    return reinterpret_cast<const char*>(glGetString(name));
}

int main() {
    glfwSetErrorCallback(on_glfw_error);

    if (!glfwInit()) {
        return 1;
    }
    std::printf("GLFW:   %s\n", glfwGetVersionString());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluid Sim", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gl_load()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::printf("OpenGL: %s\n", gl_string(GL_VERSION));
    std::printf("GLSL:   %s\n", gl_string(GL_SHADING_LANGUAGE_VERSION));
    std::printf("GPU:    %s\n", gl_string(GL_RENDERER));

    glfwSwapInterval(1);

    GLuint program = create_program("triangle.vert", "triangle.frag");
    if (!program) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    const Vertex vertices[] = {
        {-0.6f, -0.5f},
        {0.6f, -0.5f},
        {0.0f, 0.6f},
    };

    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void*>(offsetof(Vertex, x)));
    gl_check("sozlash");

    double fps_time = glfwGetTime();
    int fps_frames = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int fb_width = 0;
        int fb_height = 0;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        glViewport(0, 0, fb_width, fb_height);

        glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        gl_check("kadr");

        glfwSwapBuffers(window);

        fps_frames++;
        const double now = glfwGetTime();
        if (now - fps_time >= 0.5) {
            int win_width = 0;
            int win_height = 0;
            glfwGetWindowSize(window, &win_width, &win_height);

            char title[128];
            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS | oyna %dx%d | framebuffer %dx%d",
                          fps_frames / (now - fps_time), win_width, win_height, fb_width, fb_height);
            glfwSetWindowTitle(window, title);

            fps_time = now;
            fps_frames = 0;
        }
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

</details>

## 4-qadam. Har bir uchga rang

Oxirgi qadamda har bir uchga o'z rangini beramiz. Rang ham o'rin kabi buffer'dan keladi, lekin uni fragment shader ishlatadi. Demak rang vertex
shader'dan fragment shader'ga o'tishi kerak, oradagi piksellar esa rangni uchlardan "aralashtirib" oladi.

### 4.1. Bosqichlar orasidagi aloqa: `in` va `out`

Qiymat bosqichdan bosqichga ikki so'z bilan o'tadi:

- **`in`**: bosqichga kiradigan qiymat. Vertex shader'ning `in` lari VBO'dan keladi (3-qadam).
- **`out`**: bosqichdan chiqadigan qiymat. Vertex shader'ning `out` i fragment shader'ning **xuddi shu nomli va xuddi shu turdagi** `in` iga boradi.

Estafeta yugurishini eslang: tayoqcha bir yuguruvchidan ikkinchisiga o'tadi. `out` va `in` shu tayoqcha, nom esa kelishilgan joy. Biz uni `v_color`
deb ataymiz: `v_` "bosqichlar orasida yuradi" degani (yana faqat bizning kelishuv).

### 4.2. Interpolatsiya

Vertex shader 3 marta ishlaydi va 3 ta rang chiqaradi. Fragment shader esa 152 ming marta ishlaydi. Oradagi piksellar rangni qayerdan oladi?
Rasterizer uchala uchning rangini **aralashtiradi**: piksel qaysi uchga yaqin bo'lsa, o'shaning hissasi katta. Buni **og'irlikli o'rtacha** deyiladi.

Oddiy o'rtacha: `(2 + 4 + 6) / 3 = 4`, hamma son bir xil hissa qo'shadi. Og'irlikli o'rtachada esa ba'zilarining hissasi kattaroq. Sonlar bilan ko'ramiz.
Uchlarning ranglari (0..1 da):

```
chap past: (1.00, 0.25, 0.25)   qizil
o'ng past: (0.25, 1.00, 0.25)   yashil
tepa:      (0.25, 0.45, 1.00)   ko'k
```

- **Pastki tomonning o'rtasi** qizil va yashil uchning qoq o'rtasida, ikkalasi teng hissa qo'shadi:
  `((1.00 + 0.25) / 2, (0.25 + 1.00) / 2, (0.25 + 0.25) / 2) = (0.625, 0.625, 0.25)`, 0..255 da `(159, 159, 64)`.
- **Uchburchakning markazi** uchala uchdan bir xil "uzoqlikda", uchalasi teng: `((1 + 0.25 + 0.25) / 3, (0.25 + 1 + 0.45) / 3, (0.25 + 0.25 + 1) / 3) =
  (0.5, 0.567, 0.5)`, ya'ni `(127, 145, 127)`.

Ranglar silliq o'tishining sababi shu. 4.6-bo'limda bu sonlarni ekrandan o'lchaymiz.

### 4.3. `struct` kattalashadi

Har bir uchga uchta son qo'shiladi: `r`, `g`, `b`. Endi bitta uch 5 ta `float`, ya'ni `5 × 4 = 20` bayt. Uchta uch jami `3 × 20 = 60` bayt:

```
bayt:   0    4    8    12   16   20   24   28   ...
        ┌────┬────┬────┬────┬────┬────┬────┬────┐
        │ x  │ y  │ r  │ g  │ b  │ x  │ y  │ r  │ ...
        └────┴────┴────┴────┴────┴────┴────┴────┘
        │◄──── 0-uch ──────────►│◄──── 1-uch ...
```

Endi `sizeof(Vertex)` 20 ga, `offsetof(Vertex, r)` esa 8 ga teng (`x` va `y` dan keyin). Ikkala attribute'ning tavsifi:

| Parametr | o'rin (position) | rang (color) |
|---|---|---|
| `index` | 0 | 1 |
| `size` | 2 | 3 |
| `stride` | 20 | 20 |
| `pointer` | 0 | 8 |

GPU `n`-uchning rangini `n × stride + pointer` dan topadi: 2-uchning rangi `2 × 20 + 8 = 48`-baytdan boshlanadi. Rasmga qarab tekshirib ko'ring.

O'rin qatori o'zgarmaydi: unda `sizeof(Vertex)` va `offsetof(Vertex, x)` yozilgan, ular o'zi to'g'ri qiymatni beradi.

### 4.4. Kod: `src/main.cpp`

`Vertex` ga rang maydonlari:

<!-- diff t3 t4 src/main.cpp -->
```diff
 struct Vertex {
     float x, y;
+    float r, g, b;
 };
```

Har bir uchga rang (4.2-bo'limdagi sonlar):

<!-- diff t3 t4 src/main.cpp -->
```diff
     const Vertex vertices[] = {
-        {-0.6f, -0.5f},
-        {0.6f, -0.5f},
-        {0.0f, 0.6f},
+        {-0.6f, -0.5f, 1.0f, 0.25f, 0.25f},
+        {0.6f, -0.5f, 0.25f, 1.0f, 0.25f},
+        {0.0f, 0.6f, 0.25f, 0.45f, 1.0f},
     };
```

O'rin attribute'idan keyin rang attribute'i:

<!-- diff t3 t4 src/main.cpp -->
```diff
                           reinterpret_cast<const void*>(offsetof(Vertex, x)));
+
+    glEnableVertexAttribArray(1);
+    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
+                          reinterpret_cast<const void*>(offsetof(Vertex, r)));
     gl_check("sozlash");
```

### 4.5. Kod: shader'lar

`shaders/triangle.vert`: rangni buffer'dan olib, keyingi bosqichga uzating:

<!-- diff t3 t4 shaders/triangle.vert -->
```diff
 layout(location = 0) in vec2 in_position;
+layout(location = 1) in vec3 in_color;
+
+out vec3 v_color;
 
 void main() {
     gl_Position = vec4(in_position, 0.0, 1.0);
+    v_color = in_color;
 }
```

`location = 1` C++ dagi `glVertexAttribPointer(1, ...)` ga mos keladi. `v_color = in_color` rangni o'zgartirmasdan uzatadi, aralashtirishni esa
rasterizer qiladi.

`shaders/triangle.frag`: doimiy rang o'rniga kelgan rang:

<!-- diff t3 t4 shaders/triangle.frag -->
```diff
 #version 410 core
 
+in vec3 v_color;
+
 layout(location = 0) out vec4 out_color;
 
 void main() {
-    out_color = vec4(1.0, 0.5, 0.0, 1.0);
+    out_color = vec4(v_color, 1.0);
 }
```

`in vec3 v_color` vertex shader'dagi `out vec3 v_color` bilan bir xil nom va turda (4.1-bo'lim). `vec4(v_color, 1.0)` uch sonli rangga alpha qo'shadi.

### 4.6. Yig'ib ko'ring

To'q fonda rangli uchburchak: chap past burchak qizil, o'ng past yashil, tepa ko'k, orasi silliq o'tadi. Ekrandan o'lchadim:

| joy | hisob (4.2-bo'lim) | ekranda |
|---|---|---|
| markaz (640, 408) | (127, 145, 127) | (127, 145, 127) |
| pastki tomonning o'rtasi (640, 537) | (159, 159, 64) | (159, 159, 65) |
| chap past uch yonida | qizilga yaqin | (250, 67, 66) |
| o'ng past uch yonida | yashilga yaqin | (66, 251, 66) |
| tepa uch yonida | ko'kka yaqin | (68, 117, 247) |

**Baytlarda ma'no yo'qligining isboti.** `stride` ni ikkala attribute uchun 0 qilib ko'rdim. OpenGL'da 0 "sonlar orasida bo'sh joy yo'q, zich joylashgan"
degani, ya'ni o'rin uchun qadam 8 bayt bo'lib qoladi. Natija:

- 1-uchning o'rni 8–15-baytlardan o'qildi. U yerda aslida 0-uchning rangi `(1.0, 0.25)` turibdi. Uch ekranning o'ng chetiga, `(1280, 270)` pikselga ketdi.
- 2-uchning o'rni 16–23-baytlardan o'qildi: `(0.25, 0.6)`, bu `(800, 144)` piksel.

Ekranda aynan shunday qiyshiq qizg'ish uchburchak chiqdi. GPU baytlarni biz aytgan qoida bo'yicha o'qiydi, xato qoida berilsa, xato son o'qiydi.

**Nima buzilsa nima bo'ladi.** Shu qadamning kodida sinab ko'rdim:

| O'zgarish | Natija | Xabar bormi |
|---|---|---|
| ikkala `stride` 0 | qiyshiq qizg'ish uchburchak (yuqorida) | yo'q |
| `glEnableVertexAttribArray(1)` yo'q | qora uchburchak | yo'q |
| `triangle.frag` da `v_colour` deb yozilsa | qora uchburchak, link o'tadi | yo'q |
| `triangle.frag` da `in vec4 v_color` | link o'tadi, rasm odatdagidek | yo'q |

Oxirgi ikki qatorga e'tibor bering: NVIDIA drayveri kechirimli. GLSL talab qiladigan nom va tur mosligini tekshirmasdan o'tkazib yuborishi mumkin.
Apple'ning compiler'i qattiqroq, bunday kod Mac'da odatda xato beradi (buni o'zim ko'ra olmadim). Shuning uchun har bir qadamni ikkala kompyuterda
sinaymiz.

Bu qadamdan keyingi fayllar darsning yakuniy kodi:

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/gl.hpp</code> (solishtirish uchun)</summary>

```cpp
#pragma once

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif
#include <GL/glcorearb.h>

#define GL_FUNCTIONS(X)                                                \
    X(PFNGLGETERRORPROC, glGetError)                                   \
    X(PFNGLGETSTRINGPROC, glGetString)                                 \
    X(PFNGLVIEWPORTPROC, glViewport)                                   \
    X(PFNGLCLEARCOLORPROC, glClearColor)                               \
    X(PFNGLCLEARPROC, glClear)                                         \
    X(PFNGLCREATESHADERPROC, glCreateShader)                           \
    X(PFNGLSHADERSOURCEPROC, glShaderSource)                           \
    X(PFNGLCOMPILESHADERPROC, glCompileShader)                         \
    X(PFNGLGETSHADERIVPROC, glGetShaderiv)                             \
    X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)                   \
    X(PFNGLDELETESHADERPROC, glDeleteShader)                           \
    X(PFNGLCREATEPROGRAMPROC, glCreateProgram)                         \
    X(PFNGLATTACHSHADERPROC, glAttachShader)                           \
    X(PFNGLLINKPROGRAMPROC, glLinkProgram)                             \
    X(PFNGLGETPROGRAMIVPROC, glGetProgramiv)                           \
    X(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)                 \
    X(PFNGLDELETEPROGRAMPROC, glDeleteProgram)                         \
    X(PFNGLUSEPROGRAMPROC, glUseProgram)                               \
    X(PFNGLGENBUFFERSPROC, glGenBuffers)                               \
    X(PFNGLDELETEBUFFERSPROC, glDeleteBuffers)                         \
    X(PFNGLBINDBUFFERPROC, glBindBuffer)                               \
    X(PFNGLBUFFERDATAPROC, glBufferData)                               \
    X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)                     \
    X(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)               \
    X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)                     \
    X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)     \
    X(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)             \
    X(PFNGLDRAWARRAYSPROC, glDrawArrays)

#define X(type, name) extern type name;
GL_FUNCTIONS(X)
#undef X

bool gl_load();
void gl_check(const char* where);
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>shaders/triangle.vert</code> (solishtirish uchun)</summary>

```glsl
#version 410 core

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;

out vec3 v_color;

void main() {
    gl_Position = vec4(in_position, 0.0, 1.0);
    v_color = in_color;
}
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>shaders/triangle.frag</code> (solishtirish uchun)</summary>

```glsl
#version 410 core

in vec3 v_color;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(v_color, 1.0);
}
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/main.cpp</code> (solishtirish uchun)</summary>

```cpp
#include "gl.hpp"
#include "shader.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstddef>
#include <cstdio>

struct Vertex {
    float x, y;
    float r, g, b;
};

static void on_glfw_error(int code, const char* description) {
    std::fprintf(stderr, "GLFW xatosi %d: %s\n", code, description);
}

static const char* gl_string(GLenum name) {
    return reinterpret_cast<const char*>(glGetString(name));
}

int main() {
    glfwSetErrorCallback(on_glfw_error);

    if (!glfwInit()) {
        return 1;
    }
    std::printf("GLFW:   %s\n", glfwGetVersionString());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluid Sim", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    if (!gl_load()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    std::printf("OpenGL: %s\n", gl_string(GL_VERSION));
    std::printf("GLSL:   %s\n", gl_string(GL_SHADING_LANGUAGE_VERSION));
    std::printf("GPU:    %s\n", gl_string(GL_RENDERER));

    glfwSwapInterval(1);

    GLuint program = create_program("triangle.vert", "triangle.frag");
    if (!program) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    const Vertex vertices[] = {
        {-0.6f, -0.5f, 1.0f, 0.25f, 0.25f},
        {0.6f, -0.5f, 0.25f, 1.0f, 0.25f},
        {0.0f, 0.6f, 0.25f, 0.45f, 1.0f},
    };

    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void*>(offsetof(Vertex, x)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void*>(offsetof(Vertex, r)));
    gl_check("sozlash");

    double fps_time = glfwGetTime();
    int fps_frames = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int fb_width = 0;
        int fb_height = 0;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        glViewport(0, 0, fb_width, fb_height);

        glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        gl_check("kadr");

        glfwSwapBuffers(window);

        fps_frames++;
        const double now = glfwGetTime();
        if (now - fps_time >= 0.5) {
            int win_width = 0;
            int win_height = 0;
            glfwGetWindowSize(window, &win_width, &win_height);

            char title[128];
            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS | oyna %dx%d | framebuffer %dx%d",
                          fps_frames / (now - fps_time), win_width, win_height, fb_width, fb_height);
            glfwSetWindowTitle(window, title);

            fps_time = now;
            fps_frames = 0;
        }
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

</details>

## 5. Yakuniy tekshiruv

**Windows.** Bu natijani o'zim ko'rdim (1280 × 720):

- To'q fonda rangli uchburchak, 4.6-bo'limdagi ranglar bilan.
- Uchburchak 256..1023 va 145..539 piksellar orasida (2.2-bo'limdagi hisob).
- Terminalda `OpenGL xatosi` qatorlari yo'q, sarlavhada 165 FPS.
- Uchburchak eniga cho'zilgan. Bu xato emas, 2.2-bo'limda aytilganidek kutilgan holat.

**macOS.** Buni o'zim ishga tushira olmadim, siz esa ishlaganini aytgansiz. Kutilayotgan natija: xuddi shu uchburchak, oynaning xuddi shu joyida.
Oyna to'liq to'lishi `glViewport` ga framebuffer o'lchami berilganining isboti (01-dars, Retina).

**Yana uch holat** yakuniy kodda sinab ko'rilgan:

| O'zgarish | Natija | Xabar bormi |
|---|---|---|
| `glUseProgram` yo'q | qora uchburchak (2-qadamda esa umuman chizilmagan edi) | yo'q |
| `glViewport` yarim o'lchamda | rasm chap pastki choragiga siqiladi (Retina xatosi shu ko'rinishda bo'ladi) | yo'q |
| `#version 430 core` | ishlaydi | yo'q |

Oxirgi qator ham NVIDIA'ning kechirimliligi: biz 4.1 kontekst so'raganmiz, 4.30 shader esa baribir ishlaydi. Mac'da bunday shader kompilyatsiyadan
o'tmasligi kerak. Shuning uchun har doim `#version 410 core` yozamiz.

**Mac'da sinab ko'ring:** `triangle.frag` dagi ikkala `v_color` ni vaqtincha `v_colour` qiling. Windows'da uchburchak qora chiqqan edi, Mac'da nima
chiqqanini yozing.

## 6. Git va Mac

Windows'da:

```
git add .
git commit -m "Lesson 02: first triangle with shaders"
git push
```

Mac'da:

```
git pull
cmake --build build
./build/fluid_sim
```

## 7. Lug'at

| So'z | Ma'nosi |
|---|---|
| pipeline (konveyer) | GPU ma'lumotni o'tkazadigan bosqichlar ketma-ketligi |
| shader | GPU'da ishlaydigan kichik dastur |
| vertex | uchburchak uchi va unga tegishli ma'lumot (o'rin, rang) |
| fragment | "piksel bo'lishga nomzod", rasterizer topgan har bir piksel |
| rasterizer | uchburchak qaysi piksellarni qoplashini topadigan bosqich |
| NDC | -1 dan +1 gacha kvadrat, GPU'ning koordinatalari |
| clipping | kvadratdan tashqaridagi qismni kesib tashlash |
| GLSL | shader'lar yoziladigan C'ga o'xshash til |
| `vec2`, `vec3`, `vec4` | 2, 3 va 4 sonli vektor turlari |
| `gl_Position`, `gl_VertexID` | uchning NDC'dagi o'rni; nechanchi uch ekani |
| `in` / `out` | bosqichga kiradigan / bosqichdan chiqadigan qiymat |
| interpolatsiya | uchlar qiymatlarini og'irlikli o'rtacha bilan piksellarga taqsimlash |
| og'irlikli o'rtacha | ba'zi sonlar ko'proq hissa qo'shadigan o'rtacha |
| program | link qilingan vertex va fragment shader juftligi |
| bayt, `float` | xotiraning eng kichik bo'lagi; kasr son, 4 bayt |
| `struct` | bir nechta qiymatni bitta nom ostida birlashtirish |
| `sizeof`, `offsetof` | necha bayt; maydon boshidan necha bayt keyin |
| VBO | GPU xotirasidagi baytlar bloki |
| VAO | baytlarni qanday o'qishni eslab qoladigan obyekt |
| attribute | vertex shader'ning buffer'dan keladigan `in` i |
| stride | bir uchning boshidan keyingisining boshigacha necha bayt |
| bind | obyektni "ochiq daftar" qilish, keyingi buyruqlar unga tegadi |

## Keyingi dars

03-darsda uchburchakdan zarrachaga o'tamiz: dunyo koordinatalarini ekranga o'tkazadigan orthographic proyeksiya, bitta chizish buyrug'i bilan minglab nusxa
(instancing) va silliq chetli doira.

Shu dars tugagach yozing. Xohlasangiz, kodingizni ko'rib chiqaman.
