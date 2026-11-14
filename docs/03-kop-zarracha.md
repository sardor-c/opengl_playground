# 03. Ko'p zarracha: dunyo koordinatalari, instancing va silliq doira

> 2026-11-13 da bu dars qadam-baqadam qo'llanma ko'rinishiga o'tkazildi: har bir qadamda avval nazariya, keyin shu qadamning
> kodi va uni qayerga yozish, oxirida yig'ib tekshirish. **Yakuniy kod o'zgarmagan.** 03-darsni bajarib bo'lgansiz, loyihangizda
> hech narsani almashtirish shart emas. Har bir oraliq qadamning kodi alohida yig'ilib, ishga tushirib tekshirildi.

## Bu darsda nima qo'shamiz va nega aynan hozir

02-darsda bitta uchburchakni NDC koordinatalarida chizdik. Suyuqlik uchun esa minglab doira kerak, ularning o'rni fizika birliklarida bo'ladi.
Oldimizda uchta muammo turibdi va dars shu uchtasini hal qiladi:

1. NDC fizika uchun noqulay va rasmni cho'zadi: **dunyo koordinatalari va proyeksiya** kerak.
2. Minglab obyektni bittalab chizish sekin: **instancing** kerak.
3. GPU doira chizishni bilmaydi: **kvadrat, fragment shader va blending** kerak.

Dars oxirida ekranda 4536 ta doira turadi. 04-darsdan boshlab ular harakatlana boshlaydi, chizish kodi esa deyarli o'zgarmaydi.

Oldindan matematika bilish shart emas. Har bir tushuncha noldan tushuntiriladi: avval kundalik misol, keyin kichik sonlar, oxirida kodda qayerda
turgani. Dars oxirida qisqa lug'at bor.

Loyihada shular o'zgaradi (`CMakeLists.txt` ga tegmaymiz):

```
opengl_fluid_sim_scratch/
├── shaders/
│   ├── particle2d.vert     (yangi)
│   ├── particle2d.frag     (yangi)
│   ├── triangle.vert       (o'chiriladi)
│   └── triangle.frag       (o'chiriladi)
└── src/
    ├── gl.hpp              (o'zgaradi)
    ├── shader.hpp          (o'zgaradi)
    ├── shader.cpp          (o'zgaradi)
    └── main.cpp            (o'zgaradi)
```

## Dars qanday tuzilgan

Dars 3 qadamdan iborat. Har bir qadamda: **nazariya**, keyin shu qadamning **kodi** va uni qayerga yozish, keyin **yig'ib ko'ring**.
Har bir qadamdan keyin dastur yig'iladi va ishlaydi.

| Qadam | Nima qilamiz | Natija |
|---|---|---|
| 1 | Dunyo koordinatalari va uniform | 02-darsdagi uchburchak kichrayadi va endi cho'zilmaydi |
| 2 | Kvadrat zarrachalar va instancing | 4536 ta kvadratdan iborat rangli to'r |
| 3 | Silliq doira va blending | kvadratlar silliq chetli doiraga aylanadi |

Kod oldingi darslardagidek ko'rsatiladi. Yangi fayl to'liq beriladi. Mavjud faylga kiritiladigan o'zgarish **diff** ko'rinishida beriladi:
`-` bilan boshlangan qatorni o'chiring, `+` bilan boshlanganini qo'shing. Belgisiz qatorlar joyni topish uchun, ularga tegmang.
`+` va `-` belgisining o'zini yozmang. Har bir qadam oxirida o'zgargan faylning to'liq holati "▶" belgili yashirin blokda turadi.

Yig'ish va ishga tushirish har qadamda bir xil:

```
cmake --build build
.\build\fluid_sim.exe      # Windows (PowerShell)
./build/fluid_sim          # macOS
```

**Boshlanish nuqtasi** 02-darsning oxiridagi holat: to'q fonda rangli uchburchak.

## 1-qadam. Dunyo koordinatalari va uniform

Birinchi muammodan boshlaymiz: chizishni fizika birliklariga o'tkazamiz. Buni hali 02-darsdagi uchburchak ustida qilamiz, shunda o'zgarishni
aniq ko'rasiz.

### 1.1. Dunyo koordinatalari

**Muammo.** Fizikada o'lchamlar pikselda emas. Keyingi darslarda idish 17.1 × 9.3 birlik, gravitatsiya −12, zarracha diametri 0.08 bo'ladi. Oyna
kattalashsa yoki kichraysa, fizika o'zgarmasligi kerak. Shuning uchun simulyatsiya o'z birliklarida yashaydi. Bular **dunyo koordinatalari** deyiladi.
Ularni ekranga o'tkazish esa faqat chizishning ishi.

**Xarita misoli.** Xaritada "1 sm = 1 km" deb yozilgan bo'ladi. Shahar o'zgarmaydi, xaritani kattaroq qog'ozga chizsangiz, faqat masshtab o'zgaradi. Biz ham
dunyoni ekranga masshtab bilan chizamiz. 1280 × 720 oynada u "1 birlik = 72 piksel" bo'lib chiqadi.

**Dunyoning qaysi qismi ko'rinadi.** Buni o'zimiz tanlaymiz: markazi `(0, 0)`, balandligining yarmi `half_height = 5`, ya'ni pastdan yuqoriga −5 dan 5 gacha
ko'rinadi. Eni esa oyna shakliga bog'liq:

```
         dunyo                                  NDC
(-8.89, 5) ─────────── (8.89, 5)      (-1, 1) ───────── (1, 1)
    │                     │               │                │
    │       (0, 0)        │     ──►       │     (0, 0)     │
    │                     │               │                │
(-8.89,-5) ─────────── (8.89,-5)      (-1,-1) ───────── (1,-1)
```

**O'tkazish bu bo'lish.** Ko'rinadigan sohaning cheti NDC'da ±1 bo'lishi kerak. Shuning uchun dunyo koordinatasini ko'rinadigan yarim o'lchamga bo'lamiz:

```
clip.x = world.x / half_width
clip.y = world.y / half_height
```

Bunday o'tkazish **orthographic proyeksiya** deyiladi: uzoqlik o'lchamga ta'sir qilmaydi, perspektiva yo'q. Xarita yoki qurilish chizmasi shunday, fotosurat esa
boshqacha (uzoqdagi narsa kichik chiqadi, 15-dars). To'liq shakli matritsa bo'ladi, lekin markazi nolda turgan 2D kamera uchun ikkita son yetadi. Shuning uchun
shader'ga bitta `vec2` beramiz: `(1 / half_width, 1 / half_height)`. Bo'lish o'rniga teskari songa ko'paytiramiz: `x / 8.889` bilan `x × 0.1125` bir xil,
ko'paytirish esa GPU uchun arzonroq.

**`half_width` ni nega hisoblaymiz.** NDC kvadrati butun oynaga cho'ziladi, 1280 × 720 oynada esa eni uzunroq. Doira doira bo'lib qolishi uchun 1 dunyo birligi
ikkala o'qda bir xil piksel bo'lishi kerak. Buning uchun **aspect** kerak: oynaning eni bo'yidan necha baravar katta ekani.

```
aspect       = 1280 / 720          = 1.778
half_width   = 5 × 1.778           = 8.889
1 birlik (x) = (1280 / 2) / 8.889  = 72 piksel
1 birlik (y) = (720 / 2) / 5       = 72 piksel
```

`aspect` ni framebuffer o'lchamidan olamiz, chunki `glViewport` ga ham shu beriladi (01-dars). **Nolga bo'lish** haqida: oyna kichraytirilganda (minimize)
framebuffer `0 × 0` bo'ladi, buni sarlavhada o'z ko'zim bilan ko'rdim. Biror sonni 0 ga bo'lishning javobi yo'q, kompyuter esa bunday holatda "cheksizlik"
yoki "son emas" degan buzuq qiymat beradi. `std::max(fb_height, 1)` ikki sondan kattasini oladi, shuning uchun bo'luvchi hech qachon 0 bo'lmaydi.

**Uchburchak bilan misol.** 02-darsdagi uchburchak uchlarining sonlari o'zgarmaydi, lekin endi ular dunyo birliklari: eni `0.6 - (-0.6) = 1.2` birlik, bo'yi
`0.6 - (-0.5) = 1.1` birlik. Ekranda bu `1.2 × 72 = 86.4` va `1.1 × 72 = 79.2` piksel bo'ladi. Tepa uchi `(0, 0.6)`:

```
clip     = (0 / 8.889, 0.6 / 5) = (0, 0.12)
piksel_y = 360 - 0.12 × 360 = 316.8          (02-darsdagi formula)
```

### 1.2. Uniform: hamma uchun bitta qiymat

02-darsdagi **attribute** har bir uch yoki zarracha uchun o'z qiymatini oladi, xuddi har bir o'quvchining shaxsiy kartochkasi kabi. Proyeksiya esa hamma uchun
bir xil, uni har bir uch bilan saqlash isrof bo'lardi. Buning uchun **uniform** bor: e'lonlar taxtasidagi yozuv kabi, hamma bitta qiymatni o'qiydi. U C++ dan
beriladi va bitta chizish buyrug'i davomida o'zgarmaydi.

```glsl
uniform vec2 u_world_to_clip;      // shader'da
```

```cpp
GLint loc = glGetUniformLocation(program, "u_world_to_clip");   // nomdan raqam, bir marta
glUniform2f(loc, x, y);                                         // qiymatni berish
```

Uniform'ga nom bilan emas, raqam (**location**) bilan murojaat qilinadi. Raqamni nom bo'yicha so'raymiz. OpenGL 4.3 da raqamni shader'ning o'zida
`layout(location = N)` bilan yozish mumkin, bizda esa 4.1. `glUniform2f` dagi `2f` "ikkita `float`" degani.

Uchta nozik joy bor:

- `glUniform*` **hozir ishlatilayotgan** program'ga yozadi. `glUseProgram` dan oldin chaqirilsa, `GL_INVALID_OPERATION` beriladi. Shuning uchun avval
  `glUseProgram`, keyin `glUniform*`.
- Qiymat program ichida saqlanadi va keyingi `glUniform*` gacha turadi.
- Nom topilmasa, `glGetUniformLocation` −1 qaytaradi, `glUniform*(-1, ...)` esa **jimgina e'tiborsiz qoldiriladi**. Bundan tashqari, compiler shader'da
  ishlatilmagan uniform'ni olib tashlaydi va bunda nom to'g'ri bo'lsa ham −1 chiqadi. Natija: bo'sh ekran va hech qanday xabar yo'q. Shuning uchun kichik
  `uniform_location` funksiyasini yozamiz, u terminalga xabar chiqaradi.

### 1.3. Kod: `src/gl.hpp`

Ro'yxat oxiriga ikki funksiya. `glDrawArrays` qatori endi oxirgi emas, unga `\` qo'shiladi:

<!-- diff r0 r1 src/gl.hpp -->
```diff
-    X(PFNGLDRAWARRAYSPROC, glDrawArrays)
+    X(PFNGLDRAWARRAYSPROC, glDrawArrays)                               \
+    X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)               \
+    X(PFNGLUNIFORM2FPROC, glUniform2f)
```

### 1.4. Kod: `src/shader.hpp` va `src/shader.cpp`

`shader.hpp` da `create_program` e'lonining tagiga:

<!-- diff r0 r1 src/shader.hpp -->
```diff
 GLuint create_program(const char* vertex_file, const char* fragment_file);
+
+GLint uniform_location(GLuint program, const char* name);
```

`shader.cpp` ning oxiriga, `create_program` dan keyin:

<!-- append r0 r1 src/shader.cpp -->
```cpp

GLint uniform_location(GLuint program, const char* name) {
    const GLint location = glGetUniformLocation(program, name);
    if (location == -1) {
        std::fprintf(stderr, "Uniform topilmadi: %s\n", name);
    }
    return location;
}
```

`glGetUniformLocation` ni chaqiradi va −1 chiqsa, terminalga nomini yozadi (1.2-bo'lim). Raqamni baribir qaytaradi: −1 bilan chaqirilgan `glUniform*` hech
narsani buzmaydi, faqat ishlamaydi, biz esa endi buni bilamiz. `fprintf` uchun `#include <cstdio>` fayl boshida 02-darsdan beri bor.

### 1.5. Kod: `shaders/triangle.vert`

Uniform'ni e'lon qiling va o'rinni unga ko'paytiring:

<!-- diff r0 r1 shaders/triangle.vert -->
```diff
 layout(location = 1) in vec3 in_color;
 
+uniform vec2 u_world_to_clip;
+
 out vec3 v_color;
 
 void main() {
-    gl_Position = vec4(in_position, 0.0, 1.0);
+    gl_Position = vec4(in_position * u_world_to_clip, 0.0, 1.0);
     v_color = in_color;
```

GLSL'da ikkita `vec2` ni ko'paytirish komponentma-komponent bo'ladi: `x` `x` ga, `y` `y` ga. Bu 1.1-bo'limdagi formulaning o'zi, faqat bo'lish
o'rniga teskari songa ko'paytirish. `u_` "uniform" degani (bizning kelishuv).

### 1.6. Kod: `src/main.cpp`

`std::max` uchun `<algorithm>`:

<!-- diff r0 r1 src/main.cpp -->
```diff
 
+#include <algorithm>
 #include <cstddef>
```

Program yaratilgandan keyin uniform'ning raqamini bir marta so'rang. Raqam program link qilingandan keyin o'zgarmaydi:

<!-- diff r0 r1 src/main.cpp -->
```diff
     }
 
+    const GLint loc_world_to_clip = uniform_location(program, "u_world_to_clip");
+
     const Vertex vertices[] = {
```

Kadr ichida proyeksiyani hisoblab, `glUseProgram` dan **keyin** bering:

<!-- diff r0 r1 src/main.cpp -->
```diff
         glClear(GL_COLOR_BUFFER_BIT);
 
+        const float half_height = 5.0f;
+        const float aspect = static_cast<float>(std::max(fb_width, 1)) / std::max(fb_height, 1);
+        const float half_width = half_height * aspect;
+
         glUseProgram(program);
+        glUniform2f(loc_world_to_clip, 1.0f / half_width, 1.0f / half_height);
+
         glBindVertexArray(vao);
```

`aspect` har kadrda hisoblanadi, chunki oyna o'lchami istalgan payt o'zgarishi mumkin. `static_cast<float>` bo'lishni butun sonlarda emas, kasr sonlarda
bajarish uchun: butun sonlar bo'linmasi kasr qismini tashlab yuboradi (`1280 / 720` butun sonlarda `1` bo'lardi). `half_height = 5` kelgusi sahnaning
ko'rinadigan balandligi.

### 1.7. Yig'ib ko'ring

Uchburchak kichrayadi va oyna markazida turadi. O'lchadim: `x` bo'yicha 597 dan 682 gacha (86 piksel), `y` bo'yicha 318 dan 395 gacha (78 piksel). 1.1-bo'limdagi
hisob: 86.4 va 79.2 piksel, tepasi 316.8 da. Ranglar 02-darsdagidek.

Endi uchburchak cho'zilmaydi. Buni oyna shaklini o'zgartirib tekshiring: oynani 900 × 900 qilganimda uchburchak 108 × 98 piksel bo'ldi. 1 birlik endi
`450 / 5 = 90` piksel: `1.2 × 90 = 108`, `1.1 × 90 = 99`. Eni va bo'yining nisbati avvalgidek.

**Sinab ko'ring:** `half_height = 5.0f` ni `1.0f` qiling. Ko'rinadigan soha 5 baravar kichrayadi, ya'ni rasm 5 baravar kattalashadi. O'lchadim: 432 × 395 piksel.
Bo'yi 02-darsdagidek (395), eni esa 768 emas, 432: cho'zilish yo'q. Sinab bo'lgach, `5.0f` ni qaytaring.

**Nima buzilsa nima bo'ladi.** Shu qadamning kodida sinab ko'rdim:

| O'zgarish | Natija | Xabar bormi |
|---|---|---|
| `half_width = half_height` | uchburchak eniga 1.78 baravar cho'zilgan (152 piksel) | yo'q |
| uniform nomida xato (`"u_world_to_clp"`) | ekran bo'sh, FPS joyida | `Uniform topilmadi: u_world_to_clp` |
| shader `u_world_to_clip` ni ishlatmaydi | nom to'g'ri bo'lsa ham −1, uchburchak 02-darsdagidek katta | `Uniform topilmadi: u_world_to_clip` |
| `glUniform2f` `glUseProgram` dan oldin | rasm to'g'ri | bir marta `GL_INVALID_OPERATION` |

Ikkinchi qatorda ekran nega bo'sh? `glUniform2f(-1, ...)` e'tiborsiz qoldirildi, uniform esa boshlang'ich qiymatda qoldi: `(0, 0)`. Hamma uch nolga
ko'paytirilib, bitta nuqtaga tushdi.

Oxirgi qatorga ayniqsa e'tibor bering. Birinchi kadrda hali hech qaysi program ishlatilmayapti, shuning uchun xato. Keyingi kadrlarda esa oldingi kadrdan
qolgan program hali o'rnatilgan, shuning uchun hammasi ishlaydi. Xabar terminalda bir marta chiqib o'tib ketadi, rasm esa to'g'ri ko'rinadi. Xabarni
o'qimasangiz, xatoni sezmaysiz.

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
    X(PFNGLDRAWARRAYSPROC, glDrawArrays)                               \
    X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)               \
    X(PFNGLUNIFORM2FPROC, glUniform2f)

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

uniform vec2 u_world_to_clip;

out vec3 v_color;

void main() {
    gl_Position = vec4(in_position * u_world_to_clip, 0.0, 1.0);
    v_color = in_color;
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

#include <algorithm>
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

    const GLint loc_world_to_clip = uniform_location(program, "u_world_to_clip");

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

        const float half_height = 5.0f;
        const float aspect = static_cast<float>(std::max(fb_width, 1)) / std::max(fb_height, 1);
        const float half_width = half_height * aspect;

        glUseProgram(program);
        glUniform2f(loc_world_to_clip, 1.0f / half_width, 1.0f / half_height);

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

## 2-qadam. Ko'p zarracha: kvadrat va instancing

Uchburchak o'rniga endi 4536 ta zarracha chizamiz. Har bir zarracha hozircha kichik kvadrat bo'ladi, doiraga 3-qadamda aylantiramiz.

### 2.1. Zarracha shakli: kvadrat va `gl_VertexID`

GPU uchburchak chizadi, doirani emas. Eng sodda yo'l: har bir zarrachani ikki uchburchakdan iborat kvadrat (**quad**) qilib chizamiz, doirani esa fragment
shader'da kesib olamiz (3-qadam). Ko'p burchakli "doira" yasash ham mumkin edi, lekin har bir zarrachaga o'nlab uch ketardi va cheti baribir qirrali bo'lardi.

Kvadratning oltita uchi (ikki uchburchak, har biri 3 ta uch) hamma zarrachada bir xil, shuning uchun ularni VBO'ga yozmaymiz. 02-darsdagi **`gl_VertexID`**
yana yordamga keladi: hozirgi uchning tartib raqami, 0 dan 5 gacha. U jadvaldan burchakni tanlaydi:

```
5 (-1, 1) ─────── 2, 4 (1, 1)
    │            ╱ │
    │         ╱    │
    │      ╱       │
    │   ╱          │
0, 3 (-1,-1) ───── 1 (1,-1)
```

Jadval GLSL massivi `CORNERS[6]`. **Massiv** bir xil turdagi qiymatlar qatori, xuddi raqamlangan qutilar javoni. Raqamlash 0 dan boshlanadi: `CORNERS[0]` birinchi
quti, `CORNERS[5]` oltinchisi. 0, 1, 2-uchlar birinchi uchburchak, 3, 4, 5 ikkinchisi.

Uchning dunyodagi o'rni:

```glsl
vec2 world = in_position + corner * 0.5 * u_diameter;
```

`in_position` zarrachaning markazi, `corner` −1 dan 1 gacha, `0.5 * u_diameter` kvadratning yarim o'lchami. Misol: zarracha `(1, 2)` da, diametri 0.08, yarmi 0.04.

```
corner ( 1,  1):  (1, 2) + ( 1,  1) × 0.04 = (1.04, 2.04)     o'ng yuqori burchak
corner (-1, -1):  (1, 2) + (-1, -1) × 0.04 = (0.96, 1.96)     chap pastki burchak
```

Diametr ham hamma zarracha uchun bir xil, shuning uchun u ham uniform: `u_diameter`.

### 2.2. Instancing: bitta buyruq, minglab nusxa

Eng oddiy yo'l har bir zarrachani alohida chizish:

```cpp
for (const Particle& p : particles) {
    glUniform2f(loc_position, p.x, p.y);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
```

Har bir chizish buyrug'i (**draw call**) drayver ichida CPU uchun ish: holatni tekshirish, buyruqni GPU navbatiga qo'yish. Do'konga har bir mahsulot uchun alohida
borishni tasavvur qiling: 4536 marta borib-kelish. Yo'lning o'zi xaridning o'zidan uzoq vaqt oladi. Zarracha ko'payganda GPU emas, CPU tiqilib qoladi.

**Instancing** bitta buyruq bilan "shu 6 ta uchni N marta chiz" deydi, ya'ni bitta ro'yxat bilan bir marta borish:

```cpp
glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particle_count);
```

Parametrlar `glDrawArrays` dagidek, faqat oxirida nusxalar soni (**instance**, "nusxa") qo'shilgan.

**Har bir nusxa o'z o'rnini qayerdan biladi?** **Divisor** orqali:

```cpp
glVertexAttribDivisor(0, 1);
```

Bu "0-attribute qanchalik tez yuradi" degani. Divisor 0 (odatiy, 02-darsdagidek): har bir uchda buffer'dagi keyingi element. Divisor 1: har bir nusxada
keyingi element.

```
VBO:         P0  P1  P2  P3  P4  P5  P6 ...

divisor 0:   uch 0 → P0, uch 1 → P1, ... uch 5 → P5     (hamma nusxada bir xil)

divisor 1:   nusxa 0: oltala uch → P0
             nusxa 1: oltala uch → P1
             nusxa 2: oltala uch → P2
```

02-darsdagi `struct` va `glVertexAttribPointer` aynan o'zi qoladi, baytlar joylashuvi o'zgarmaydi, faqat "qadam" o'zgaradi. Divisor ham VAO ichida saqlanadi.

**O'lchov** (RTX 3060, i5-12400F, vsync o'chiq, 1280 × 720). O'ng ustunda har bir zarracha uchun ikkita `glUniform*` va bitta `glDrawArrays` chaqiriladi:

| Zarracha | Instancing | Alohida draw call |
|---|---|---|
| 4 536 | ~10 000 FPS | ~930 FPS |
| 40 000 | ~6 900 FPS | 132 FPS |
| 250 000 | ~1 640 FPS | 23 FPS |
| 1 000 000 | 417 FPS | o'lchanmadi |

Vertex shader baribir har bir zarracha uchun 6 marta ishlaydi, instancing GPU ishini kamaytirmaydi. U draw call'lar sonini N dan 1 ga tushiradi, ya'ni CPU
tomonidagi ishni olib tashlaydi. 250 000 zarrachada farq 70 baravar.

### 2.3. Zarrachalar C++ da: `std::vector` va to'r

**`std::vector<Particle>`** o'lchami oldindan ma'lum bo'lmagan massiv. 02-darsdagi oddiy massiv (`Vertex vertices[]`) o'lchamini kod yozilayotganda bilishimiz
kerak edi, `vector` esa dastur ishlayotganda o'sadi. `push_back` oxiriga bitta element qo'shadi, `size()` nechta element borligini, `data()` esa elementlar
xotirada qayerdan boshlanishini beradi. **`reserve`** vektor uchun xotirani oldindan oladi. Busiz `push_back` vaqti-vaqti bilan kattaroq joy ajratib, hamma
elementni u yerga ko'chiradi.

**`make_grid`** zarrachalarni tekis to'r qilib joylaydi. `tx = i / (nx - 1)` zarrachaning qatordagi joyini 0 dan 1 gacha son bilan aytadi: birinchisi (`i = 0`) 0,
oxirgisi (`i = 80`) `80 / 80 = 1`, o'rtadagisi (`i = 40`) 0.5. Keyin:

- `(tx - 0.5) × width` uni markaz atrofiga qo'yadi: `tx = 0` da `-0.5 × 6.42 = -3.21`, `tx = 1` da `3.21`.
- Rang ham shu sondan: qizil `0.2 + 0.8 × tx` chap chetda 0.2, o'ngda 1.0. Yashil `ty` dan, ko'k `1.0 - 0.7 × tx` chapdan o'ngga kamayadi. Shuning uchun burchaklar har xil
  rangda: chap past ko'k, chap tepa moviy, o'ng past qizil, o'ng tepa sariq.

`nx - 1.0f` dagi `.0f` muhim: `i / (nx - 1)` ikkita butun son bo'linmasi bo'lib, kasr qismini tashlab yuborardi (`40 / 80 = 0`). `1.0f` bilan bo'lish kasr sonda bajariladi.
81 × 56 va 6.42 × 4.39 sonlari keyingi darslardagi birinchi sahnadan olingan, suyuqlik aynan shu to'rdan boshlanadi.

### 2.4. Kod: shader'lar

`shaders/triangle.vert` ni o'chiring. `shaders/triangle.frag` ni `shaders/particle2d.frag` deb qayta nomlang: ichi o'zgarmaydi, hozircha har bir piksel
zarrachaning rangini oladi. Uning mazmuni shu bo'lishi kerak:

<!-- file r2 shaders/particle2d.frag -->
```glsl
#version 410 core

in vec3 v_color;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(v_color, 1.0);
}
```

Yangi `shaders/particle2d.vert`:

<!-- file r2 shaders/particle2d.vert -->
```glsl
#version 410 core

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;

uniform vec2 u_world_to_clip;
uniform float u_diameter;

out vec3 v_color;

const vec2 CORNERS[6] = vec2[6](
    vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0),
    vec2(-1.0, -1.0), vec2(1.0, 1.0), vec2(-1.0, 1.0)
);

void main() {
    vec2 corner = CORNERS[gl_VertexID];
    vec2 world = in_position + corner * 0.5 * u_diameter;
    gl_Position = vec4(world * u_world_to_clip, 0.0, 1.0);

    v_color = in_color;
}
```

**`CORNERS`** 2.1-bo'limdagi oltita burchak jadvali.

**`world * u_world_to_clip`** 1-qadamdagi proyeksiya, endi uchburchak uchi emas, kvadrat burchagi uchun.

**`in_position` va `in_color`** endi har bir nusxa uchun bitta qiymat oladi (divisor 1), oltala uch uchun bir xil.

### 2.5. Kod: `src/gl.hpp`

<!-- diff r1 r2 src/gl.hpp -->
```diff
     X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)               \
-    X(PFNGLUNIFORM2FPROC, glUniform2f)
+    X(PFNGLUNIFORM1FPROC, glUniform1f)                                 \
+    X(PFNGLUNIFORM2FPROC, glUniform2f)                                 \
+    X(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor)             \
+    X(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced)
```

`glUniform1f` "bitta `float`" (diametr uchun). Uchalasi OpenGL 4.1 da bor: `glDrawArraysInstanced` 3.1 dan, `glVertexAttribDivisor` 3.3 dan.

### 2.6. Kod: `src/main.cpp`

**Include'lar va tuzilma.** `<vector>` ni qo'shing va `Vertex` ni `Particle` deb qayta nomlang: endi u bitta uchni emas, bitta zarrachani tasvirlaydi.
Maydonlar o'sha-o'sha, 20 bayt:

<!-- diff r1 r2 src/main.cpp -->
```diff
 #include <cstdio>
+#include <vector>
 
-struct Vertex {
+struct Particle {
     float x, y;
```

**`gl_string` dan keyin** to'r yasaydigan funksiya (2.3-bo'lim):

<!-- diff r1 r2 src/main.cpp -->
```diff
 }
 
+static std::vector<Particle> make_grid(int nx, int ny, float width, float height) {
+    std::vector<Particle> particles;
+    particles.reserve(nx * ny);
+
+    for (int j = 0; j < ny; j++) {
+        for (int i = 0; i < nx; i++) {
+            const float tx = i / (nx - 1.0f);
+            const float ty = j / (ny - 1.0f);
+
+            Particle p;
+            p.x = (tx - 0.5f) * width;
+            p.y = (ty - 0.5f) * height;
+            p.r = 0.2f + 0.8f * tx;
+            p.g = 0.2f + 0.8f * ty;
+            p.b = 1.0f - 0.7f * tx;
+            particles.push_back(p);
+        }
+    }
+    return particles;
+}
+
 int main() {
```

`for (int j = 0; j < ny; j++)` ichida `for (int i = 0; i < nx; i++)`: har bir qator (`j`) uchun shu qatordagi har bir zarracha (`i`). Jami `nx × ny` marta.

**Yangi shader fayllari:**

<!-- diff r1 r2 src/main.cpp -->
```diff
-    GLuint program = create_program("triangle.vert", "triangle.frag");
+    GLuint program = create_program("particle2d.vert", "particle2d.frag");
```

**Uchlar massivi o'rniga zarrachalar to'ri** va diametr uniform'ining raqami:

<!-- diff r1 r2 src/main.cpp -->
```diff
     const GLint loc_world_to_clip = uniform_location(program, "u_world_to_clip");
+    const GLint loc_diameter = uniform_location(program, "u_diameter");
 
-    const Vertex vertices[] = {
-        {-0.6f, -0.5f, 1.0f, 0.25f, 0.25f},
-        {0.6f, -0.5f, 0.25f, 1.0f, 0.25f},
-        {0.0f, 0.6f, 0.25f, 0.45f, 1.0f},
-    };
+    const std::vector<Particle> particles = make_grid(81, 56, 6.42f, 4.39f);
+    const int particle_count = static_cast<int>(particles.size());
```

`particles.size()` ning turi `int` emas (u juda katta sonlarni ham sig'diradigan ishorasiz tur), `glDrawArraysInstanced` esa `int` kutadi. `static_cast<int>`
shu farqni yopadi.

**Buffer va attribute'lar:**

<!-- diff r1 r2 src/main.cpp -->
```diff
     glBindBuffer(GL_ARRAY_BUFFER, vbo);
-    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
+    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW);
 
     glEnableVertexAttribArray(0);
-    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
-                          reinterpret_cast<const void*>(offsetof(Vertex, x)));
+    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle),
+                          reinterpret_cast<const void*>(offsetof(Particle, x)));
+    glVertexAttribDivisor(0, 1);
 
     glEnableVertexAttribArray(1);
-    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
-                          reinterpret_cast<const void*>(offsetof(Vertex, r)));
+    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
+                          reinterpret_cast<const void*>(offsetof(Particle, r)));
+    glVertexAttribDivisor(1, 1);
     gl_check("sozlash");
```

- `glBufferData` `particles.size() * sizeof(Particle)` bayt nusxalaydi: `4536 × 20 = 90 720` bayt. `sizeof(particles)` bu yerda ishlamaydi: u vektorning ichidagi
  ma'lumotni emas, vektor obyektining o'zini o'lchaydi. `particles.data()` ma'lumot boshlanadigan manzil.
- `glVertexAttribDivisor(0, 1)` va `(1, 1)` ikkala attribute'ni ham "har nusxada keyingi element" qiladi (2.2-bo'lim).

**Kadr ichida** diametrni bering va instancing bilan chizing:

<!-- diff r1 r2 src/main.cpp -->
```diff
         glUniform2f(loc_world_to_clip, 1.0f / half_width, 1.0f / half_height);
+        glUniform1f(loc_diameter, 0.08f);
 
         glBindVertexArray(vao);
-        glDrawArrays(GL_TRIANGLES, 0, 3);
+        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particle_count);
         gl_check("kadr");
```

`0.08` zarracha diametri, dunyo birliklarida.

**Sarlavha** endi zarrachalar sonini ham ko'rsatadi:

<!-- diff r1 r2 src/main.cpp -->
```diff
             char title[128];
-            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS | oyna %dx%d | framebuffer %dx%d",
-                          fps_frames / (now - fps_time), win_width, win_height, fb_width, fb_height);
+            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS | %d zarracha | oyna %dx%d | framebuffer %dx%d",
+                          fps_frames / (now - fps_time), particle_count, win_width, win_height, fb_width, fb_height);
             glfwSetWindowTitle(window, title);
```

### 2.7. Yig'ib ko'ring

Markazda rangli to'rtburchak chiqadi: chap past ko'k, chap tepa moviy, o'ng past qizil, o'ng tepa sariq. Sarlavhada `4536 zarracha`.

To'rtburchak yaxlit ko'rinadi, alohida kvadratlar sezilmaydi. Sababi: qo'shni zarrachalar orasi `6.42 / 80 = 0.08025` birlik, kvadratning tomoni esa
0.08 birlik, ya'ni kvadratlar bir-biriga tegib turadi. O'lchadim: to'r 468 × 322 piksel, ichida birorta ham fon piksel yo'q. Hisob:
`(6.42 + 0.08) × 72 = 468`, `(4.39 + 0.08) × 72 = 322` (0.08 ikki chetdagi zarrachalarning yarim diametrlari).

**Dunyodan pikselga, bitta zarracha misolida.** To'rning o'ng yuqori zarrachasi dunyoda `(3.21, 2.195)` da turibdi, `half_width = 8.889`, `half_height = 5`:

```
clip  = (3.21 / 8.889, 2.195 / 5) = (0.361, 0.439)
piksel_x = 640 + 0.361 × 640 = 871          (02-darsdagi formula)
piksel_y = 360 - 0.439 × 360 = 202
```

Ekrandan `(871, 202)` pikselni o'qidim: `(255, 255, 76)`, aynan shu zarrachaning sariq rangi. Qo'lda: qizil `0.2 + 0.8 × 1 = 1.0` (255), yashil ham 1.0 (255),
ko'k `1.0 - 0.7 × 1 = 0.3` (76).

**Sinab ko'ring:** `glUniform1f(loc_diameter, 0.08f)` ni `0.04f` qiling. Kvadratlar ikki baravar kichrayadi va orasida fon ko'rinadi: to'rning ichki qismida
91 876 ta fon pikseli chiqdi. Sinab bo'lgach, `0.08f` ni qaytaring.

Sinab ko'rdim: ikkala `glVertexAttribDivisor` qatori bo'lmasa, hamma 4536 nusxa bir xil P0..P5 ni o'qiydi va ekranda to'rning chap pastki burchagida 16 × 6
piksellik kichik dog' qoladi. Hech qanday xabar chiqmaydi.

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
    X(PFNGLDRAWARRAYSPROC, glDrawArrays)                               \
    X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)               \
    X(PFNGLUNIFORM1FPROC, glUniform1f)                                 \
    X(PFNGLUNIFORM2FPROC, glUniform2f)                                 \
    X(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor)             \
    X(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced)

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

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <vector>

struct Particle {
    float x, y;
    float r, g, b;
};

static void on_glfw_error(int code, const char* description) {
    std::fprintf(stderr, "GLFW xatosi %d: %s\n", code, description);
}

static const char* gl_string(GLenum name) {
    return reinterpret_cast<const char*>(glGetString(name));
}

static std::vector<Particle> make_grid(int nx, int ny, float width, float height) {
    std::vector<Particle> particles;
    particles.reserve(nx * ny);

    for (int j = 0; j < ny; j++) {
        for (int i = 0; i < nx; i++) {
            const float tx = i / (nx - 1.0f);
            const float ty = j / (ny - 1.0f);

            Particle p;
            p.x = (tx - 0.5f) * width;
            p.y = (ty - 0.5f) * height;
            p.r = 0.2f + 0.8f * tx;
            p.g = 0.2f + 0.8f * ty;
            p.b = 1.0f - 0.7f * tx;
            particles.push_back(p);
        }
    }
    return particles;
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

    GLuint program = create_program("particle2d.vert", "particle2d.frag");
    if (!program) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    const GLint loc_world_to_clip = uniform_location(program, "u_world_to_clip");
    const GLint loc_diameter = uniform_location(program, "u_diameter");

    const std::vector<Particle> particles = make_grid(81, 56, 6.42f, 4.39f);
    const int particle_count = static_cast<int>(particles.size());

    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<const void*>(offsetof(Particle, x)));
    glVertexAttribDivisor(0, 1);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<const void*>(offsetof(Particle, r)));
    glVertexAttribDivisor(1, 1);
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

        const float half_height = 5.0f;
        const float aspect = static_cast<float>(std::max(fb_width, 1)) / std::max(fb_height, 1);
        const float half_width = half_height * aspect;

        glUseProgram(program);
        glUniform2f(loc_world_to_clip, 1.0f / half_width, 1.0f / half_height);
        glUniform1f(loc_diameter, 0.08f);

        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particle_count);
        gl_check("kadr");

        glfwSwapBuffers(window);

        fps_frames++;
        const double now = glfwGetTime();
        if (now - fps_time >= 0.5) {
            int win_width = 0;
            int win_height = 0;
            glfwGetWindowSize(window, &win_width, &win_height);

            char title[128];
            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS | %d zarracha | oyna %dx%d | framebuffer %dx%d",
                          fps_frames / (now - fps_time), particle_count, win_width, win_height, fb_width, fb_height);
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

## 3-qadam. Silliq doira

Kvadratlarni doiraga aylantiramiz. Buning uchun fragment shader har bir piksel zarracha markazidan qancha uzoqda ekanini bilishi va shunga qarab shaffoflik
berishi kerak. Keyin OpenGL'ga shu shaffoflikni ishlatishni aytamiz.

### 3.1. Markazdan masofa

Vertex shader har bir burchakka uning kvadrat ichidagi o'rnini (`corner`, −1 dan 1 gacha) beradi. Biz uni `v_local` deb fragment shader'ga uzatamiz. 02-darsdagi
interpolatsiya har bir piksel uchun uning kvadrat ichidagi o'rnini beradi: markazda `(0, 0)`, chetlarda ±1.

Markazgacha masofani topish uchun to'g'ri burchakli uchburchak qoidasidan (**Pifagor teoremasi**) foydalanamiz: o'ngga `x` va yuqoriga `y` yurilgan bo'lsa,
to'g'ri chiziq bo'ylab masofa `sqrt(x × x + y × y)`. Bu yerda `sqrt` kvadrat ildiz, ya'ni qaysi son o'ziga ko'paytirilsa shu son chiqadi.

```
v_local = (0.3, 0.4):  sqrt(0.09 + 0.16) = sqrt(0.25) = 0.5     doira ichida
v_local = (0.6, 0.8):  sqrt(0.36 + 0.64) = sqrt(1.00) = 1.0     aynan chetida
v_local = (1.0, 1.0):  sqrt(1 + 1)       = 1.41                  kvadrat burchagi, doiradan tashqarida
```

GLSL'da buni `length(v_local)` bitta so'z bilan hisoblaydi. **Doira** masofasi 1 dan kichik bo'lgan nuqtalar.

### 3.2. Zinapoya muammosi va `smoothstep`

Eng sodda variant: `alpha = dist < 1.0 ? 1.0 : 0.0`, ya'ni ichkarida to'liq ko'rinadi, tashqarida umuman ko'rinmaydi. Lekin piksel kichik
kvadrat, doira cheti esa egri chiziq. Chet pikselning faqat bir qismini qoplaydi, biz esa uni yo to'liq bo'yaymiz, yo umuman bo'yamaymiz. Natijada chet zinapoya
bo'lib ko'rinadi (inglizcha *aliasing*). Katta doirada sinadim: chetida birorta ham oraliq rangli piksel yo'q. To'g'risi: chet pikseli doira uni qancha qoplasa,
shuncha ko'rinishi kerak, ya'ni oraliq shaffoflik olishi kerak.

**`smoothstep(a, b, x)`** "silliq qadam": `x` `a` dan kichik bo'lsa 0, `b` dan katta bo'lsa 1, orasida esa silliq o'tadi. Avval `x` oraliqning qayerida ekani 0..1
son qilib topiladi, keyin u egri chiziq bilan yumshatiladi:

```
t = (x - a) / (b - a), 0 dan kichigi 0 ga, 1 dan kattasi 1 ga tenglanadi
smoothstep = t × t × (3 - 2 × t)
```

| `t` | 0 | 0.25 | 0.5 | 0.75 | 1 |
|---|---|---|---|---|---|
| `smoothstep` | 0 | 0.156 | 0.5 | 0.844 | 1 |

Chetlarda sekin, o'rtada tez o'zgaradi: grafigi "S" harfiga o'xshaydi. Shuning uchun o'tish yumshoq ko'rinadi.

### 3.3. O'tish kengligi: `fwidth`

O'tish kengligi bir piksel bo'lishi kerak. Lekin bir piksel `v_local` birliklarida qancha? Bu zarracha ekranda qancha joy egallashiga bog'liq. `v_local` markazdan
chetgacha 1 ga o'zgaradi:

- radiusi 2.9 piksel bo'lgan zarrachada (bizning 0.08 diametr) bir piksel `1 / 2.9 = 0.35`;
- radiusi 21.6 piksel bo'lgan zarrachada (0.6 diametr) bir piksel `1 / 21.6 = 0.046`;
- Retina ekranda piksellar ikki baravar mayda, yana boshqa son.

**`fwidth`** shu sonni o'zi o'lchaydi. GPU fragmentlarni 2 × 2 piksellik guruhlarda hisoblaydi, shuning uchun qo'shni pikseldagi qiymatni biladi va ayirmani
olishi mumkin. `fwidth(d)` "`d` keyingi pikselga o'tganda qanchaga o'zgaradi" degani. Bu faqat fragment shader'da ishlaydi.

```glsl
float dist = length(v_local);
float pixel = max(fwidth(dist), 0.0001);
float alpha = 1.0 - smoothstep(1.0 - pixel, 1.0, dist);
```

`smoothstep` chetdan tashqarida 1 beradi, bizga esa teskarisi kerak (ichkarida 1), shuning uchun `1.0 - ...`.

Ikki tafsilot:

- **Nega oraliq `[1 - pixel, 1]`.** Kvadrat `dist = 1` chizig'ida, tomonlarining o'rtasida tugaydi, undan tashqarida fragment yo'q. Shaffoflik aynan o'sha yerda 0 ga
  yetishi kerak. Keng tarqalgan `[1 - pixel, 1 + pixel]` variantida kvadrat cheti shaffoflik 0.5 bo'lgan joyda kesiladi. Katta doirada o'lchadim: to'g'ri variantda
  doiraning chap chetidagi ustunda 4 ta xira piksel (eng kuchlisi 11%), `1 ± pixel` variantida esa 16 piksel balandlikdagi to'g'ri chiziq (65% gacha). Doiraning to'rt
  tomonida tekis kesilgan joy ko'rinadi.
- **Nega `max(..., 0.0001)`.** `a` va `b` teng bo'lsa, `smoothstep` ning natijasi GLSL'da aniqlanmagan (`b - a = 0` ga bo'linadi). Doira markazi 2 × 2 guruhning
  qoq o'rtasiga tushsa, to'rtala `dist` teng bo'lib, `fwidth` 0 chiqishi mumkin.

**Nega `fwidth`, oddiy son emas.** `pixel = 0.05` deb qotirib ko'rdim. Katta (43 piksellik) doirada natija deyarli to'g'ri, chunki u yerda bir piksel aynan 0.046.
Bizning 6 piksellik zarrachada esa bir piksel 0.35, 0.05 esa uning yettidan biri. Burchakdagi zarracha atrofida silliq piksellar 49 tadan 8 taga tushdi, chet
deyarli zinapoya bo'ldi.

### 3.4. Kod: shader'lar

`shaders/particle2d.vert`: burchakni `v_local` nomi bilan keyingi bosqichga uzating:

<!-- diff r2 r3 shaders/particle2d.vert -->
```diff
 uniform float u_diameter;
 
+out vec2 v_local;
 out vec3 v_color;
```

<!-- diff r2 r3 shaders/particle2d.vert -->
```diff
     gl_Position = vec4(world * u_world_to_clip, 0.0, 1.0);
 
+    v_local = corner;
     v_color = in_color;
```

`shaders/particle2d.frag`: `v_local` ni qabul qilib, shaffoflikni hisoblang (3.3-bo'limdagi uch qator):

<!-- diff r2 r3 shaders/particle2d.frag -->
```diff
 #version 410 core
 
+in vec2 v_local;
 in vec3 v_color;
```

<!-- diff r2 r3 shaders/particle2d.frag -->
```diff
 void main() {
-    out_color = vec4(v_color, 1.0);
+    float dist = length(v_local);
+    float pixel = max(fwidth(dist), 0.0001);
+    float alpha = 1.0 - smoothstep(1.0 - pixel, 1.0, dist);
+    out_color = vec4(v_color, alpha);
 }
```

Rang zarrachaniki, shaffoflik esa markazdan masofaga qarab.

Hozir yig'sangiz, ekranda hech narsa o'zgarmaydi: 2-qadamdagi yaxlit to'rtburchak. Sinab ko'rdim: rasm 2-qadamdagi bilan piksel-pikselgacha bir xil chiqdi.
Shaffoflik hisoblanyapti, lekin OpenGL uni hali ishlatmayapti. Keyingi bo'lim shuni hal qiladi.

### 3.5. Blending: shaffoflikni aralashtirish

Fragment shader shaffoflik (alpha) chiqaradi, lekin OpenGL odatda uni ishlatmaydi: rang framebuffer'ga to'g'ridan-to'g'ri yoziladi va biz kvadratlarni ko'ramiz.

**Blending** yoqilsa, yangi rang (*source*, "manba") ekranda turgan rang (*destination*, "manzil") bilan aralashtiriladi. Bu yana og'irlikli o'rtacha (02-dars):

```
natija = yangi_rang × alpha + eski_rang × (1 - alpha)
```

Misol: ko'k zarracha `(51, 51, 255)` fon `(13, 18, 31)` ustiga `alpha = 0.5` bilan tushadi:

```
qizil:  51 × 0.5 + 13 × 0.5 = 32
yashil: 51 × 0.5 + 18 × 0.5 = 34.5
ko'k:  255 × 0.5 + 31 × 0.5 = 143
```

`alpha = 1` da zarrachaning o'z rangi, `alpha = 0` da fon qoladi, orasida aralashma. Doira chetidagi yumshoq piksellar shunday hosil bo'ladi.

```cpp
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
```

`GL_SRC_ALPHA` "yangi rangni alpha ga ko'paytir", `GL_ONE_MINUS_SRC_ALPHA` "eski rangni `1 - alpha` ga ko'paytir". Formulaning o'zi. Keyingi nusxa oldingisining
ustiga tushadi, tartib shu.

### 3.6. Kod: `src/gl.hpp` va `src/main.cpp`

`gl.hpp`, `glDrawArrays` dan keyin:

<!-- diff r2 r3 src/gl.hpp -->
```diff
     X(PFNGLDRAWARRAYSPROC, glDrawArrays)                               \
+    X(PFNGLENABLEPROC, glEnable)                                       \
+    X(PFNGLBLENDFUNCPROC, glBlendFunc)                                 \
     X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)               \
```

`main.cpp`, attribute'lardan keyin:

<!-- diff r2 r3 src/main.cpp -->
```diff
     glVertexAttribDivisor(1, 1);
+
+    glEnable(GL_BLEND);
+    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     gl_check("sozlash");
```

Blending bir marta yoqiladi va shu holatda qoladi. OpenGL holat mashinasi: yoqilgan narsa o'chirilguncha turadi (01-dars).

### 3.7. Yig'ib ko'ring

Yaxlit to'rtburchak 4536 ta kichik doiraga aylanadi, orasida fon ko'rinadi. O'lchadim: to'r hali ham 468 × 322 piksel, ichki qismida 34 075 ta fon pikseli
paydo bo'ldi (2-qadamda 0 edi). Burchak ranglari o'zgarmadi: chap past `(51, 51, 255)` ko'k, chap tepa `(51, 255, 255)` moviy, o'ng past `(255, 51, 76)` qizil,
o'ng tepa `(255, 255, 76)` sariq.

**Sinab ko'ring:** `make_grid(9, 6, 6.42f, 4.39f)` va `glUniform1f(loc_diameter, 0.6f)` qiling. Doiralar katta bo'ladi va silliq chetini yaxshi ko'rasiz.
Keyin `glEnable(GL_BLEND)` qatorini vaqtincha o'chirib solishtiring. Sinab bo'lgach, ikkalasini qaytaring.

**Nima buzilsa nima bo'ladi.** Yakuniy kodda sinab ko'rdim:

| O'zgarish | Natija | Xabar bormi |
|---|---|---|
| `glEnable(GL_BLEND)` yo'q | doiralar o'rniga kvadratlar, rasm 2-qadamdagidek | yo'q |
| `alpha = dist < 1.0 ? 1.0 : 0.0` | chet zinapoya, oraliq rangli piksel yo'q | yo'q |
| `pixel = 0.05` (o'zgarmas) | 43 piksellik doirada deyarli to'g'ri, 6 piksellik zarrachada chet silliqlanmaydi | yo'q |
| `smoothstep(1.0 - pixel, 1.0 + pixel, dist)` | doiraning to'rt tomoni tekis kesilgan | yo'q |
| `particle2d.vert` `u_diameter` ni ishlatmasa | nom to'g'ri bo'lsa ham −1 | `Uniform topilmadi: u_diameter` |

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
    X(PFNGLDRAWARRAYSPROC, glDrawArrays)                               \
    X(PFNGLENABLEPROC, glEnable)                                       \
    X(PFNGLBLENDFUNCPROC, glBlendFunc)                                 \
    X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)               \
    X(PFNGLUNIFORM1FPROC, glUniform1f)                                 \
    X(PFNGLUNIFORM2FPROC, glUniform2f)                                 \
    X(PFNGLVERTEXATTRIBDIVISORPROC, glVertexAttribDivisor)             \
    X(PFNGLDRAWARRAYSINSTANCEDPROC, glDrawArraysInstanced)

#define X(type, name) extern type name;
GL_FUNCTIONS(X)
#undef X

bool gl_load();
void gl_check(const char* where);
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>shaders/particle2d.vert</code> (solishtirish uchun)</summary>

```glsl
#version 410 core

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;

uniform vec2 u_world_to_clip;
uniform float u_diameter;

out vec2 v_local;
out vec3 v_color;

const vec2 CORNERS[6] = vec2[6](
    vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0),
    vec2(-1.0, -1.0), vec2(1.0, 1.0), vec2(-1.0, 1.0)
);

void main() {
    vec2 corner = CORNERS[gl_VertexID];
    vec2 world = in_position + corner * 0.5 * u_diameter;
    gl_Position = vec4(world * u_world_to_clip, 0.0, 1.0);

    v_local = corner;
    v_color = in_color;
}
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>shaders/particle2d.frag</code> (solishtirish uchun)</summary>

```glsl
#version 410 core

in vec2 v_local;
in vec3 v_color;

layout(location = 0) out vec4 out_color;

void main() {
    float dist = length(v_local);
    float pixel = max(fwidth(dist), 0.0001);
    float alpha = 1.0 - smoothstep(1.0 - pixel, 1.0, dist);
    out_color = vec4(v_color, alpha);
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

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <vector>

struct Particle {
    float x, y;
    float r, g, b;
};

static void on_glfw_error(int code, const char* description) {
    std::fprintf(stderr, "GLFW xatosi %d: %s\n", code, description);
}

static const char* gl_string(GLenum name) {
    return reinterpret_cast<const char*>(glGetString(name));
}

static std::vector<Particle> make_grid(int nx, int ny, float width, float height) {
    std::vector<Particle> particles;
    particles.reserve(nx * ny);

    for (int j = 0; j < ny; j++) {
        for (int i = 0; i < nx; i++) {
            const float tx = i / (nx - 1.0f);
            const float ty = j / (ny - 1.0f);

            Particle p;
            p.x = (tx - 0.5f) * width;
            p.y = (ty - 0.5f) * height;
            p.r = 0.2f + 0.8f * tx;
            p.g = 0.2f + 0.8f * ty;
            p.b = 1.0f - 0.7f * tx;
            particles.push_back(p);
        }
    }
    return particles;
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

    GLuint program = create_program("particle2d.vert", "particle2d.frag");
    if (!program) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    const GLint loc_world_to_clip = uniform_location(program, "u_world_to_clip");
    const GLint loc_diameter = uniform_location(program, "u_diameter");

    const std::vector<Particle> particles = make_grid(81, 56, 6.42f, 4.39f);
    const int particle_count = static_cast<int>(particles.size());

    GLuint vao = 0;
    GLuint vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<const void*>(offsetof(Particle, x)));
    glVertexAttribDivisor(0, 1);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<const void*>(offsetof(Particle, r)));
    glVertexAttribDivisor(1, 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

        const float half_height = 5.0f;
        const float aspect = static_cast<float>(std::max(fb_width, 1)) / std::max(fb_height, 1);
        const float half_width = half_height * aspect;

        glUseProgram(program);
        glUniform2f(loc_world_to_clip, 1.0f / half_width, 1.0f / half_height);
        glUniform1f(loc_diameter, 0.08f);

        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particle_count);
        gl_check("kadr");

        glfwSwapBuffers(window);

        fps_frames++;
        const double now = glfwGetTime();
        if (now - fps_time >= 0.5) {
            int win_width = 0;
            int win_height = 0;
            glfwGetWindowSize(window, &win_width, &win_height);

            char title[128];
            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS | %d zarracha | oyna %dx%d | framebuffer %dx%d",
                          fps_frames / (now - fps_time), particle_count, win_width, win_height, fb_width, fb_height);
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

## 4. Yakuniy tekshiruv

**Windows.** Bu natijani o'zim ko'rdim:

- To'q fonda markazda 81 × 56 = 4536 ta kichik doiradan iborat to'r, burchak ranglari 3.7-bo'limdagidek.
- 1280 × 720 da to'r 468 × 322 pikselni egallaydi.
- Oyna shaklini o'zgartirsangiz, doiralar doiraligicha qoladi: 900 × 900 oynada to'r 584 × 402 piksel (1 birlik `450 / 5 = 90` piksel).
- Terminalda `OpenGL xatosi` yoki `Uniform topilmadi` qatorlari yo'q, sarlavhada 165 FPS va `4536 zarracha`. Oyna kichraytirilganda sarlavhada `framebuffer 0x0`.

**macOS.** Buni o'zim ishga tushira olmadim, siz esa ishlaganini aytgansiz. Kutilayotgan natija: xuddi shu to'r, oynaning xuddi shu joyida. Retina ekranda sarlavhadagi
framebuffer oynadan ikki baravar katta bo'ladi va doira chetlari aniqroq ko'rinadi, chunki `fwidth` piksel o'lchamiga o'zi moslashadi.

**Mac'da sinab ko'ring:** `make_grid(1000, 1000, 6.42f, 4.39f)` va `glfwSwapInterval(0)` qiling: million zarracha. Windows'da 417 FPS chiqdi, Mac'da qancha
chiqishini yozing.

## 5. Git va Mac

Windows'da:

```
git add .
git commit -m "Lesson 03: world coordinates, instancing, smooth circles"
git push
```

Mac'da:

```
git pull
cmake --build build
./build/fluid_sim
```

## 6. Lug'at

| So'z | Ma'nosi |
|---|---|
| dunyo koordinatalari | simulyatsiyaning o'z birliklari, ekranga bog'liq emas |
| masshtab | 1 birlik necha pikselga teng (bizda 72) |
| orthographic proyeksiya | uzoqlik o'lchamga ta'sir qilmaydigan o'tkazish, xarita kabi |
| aspect | oynaning eni bo'yidan necha baravar katta |
| uniform | bitta chizish davomida hamma uchun bir xil qiymat |
| location | uniform yoki attribute'ning raqami |
| quad | ikki uchburchakdan iborat kvadrat |
| massiv | raqamlangan qiymatlar qatori, raqamlash 0 dan |
| `gl_VertexID` | hozirgi uchning tartib raqami |
| draw call | GPU'ga bitta chizish buyrug'i |
| instancing | bitta buyruq bilan bir xil shaklni ko'p marta chizish |
| instance | bitta nusxa |
| divisor | attribute har uchda yoki har nusxada yurishi |
| `std::vector` | o'lchami dastur ishlayotganda o'zgaradigan massiv |
| Pifagor teoremasi | masofa = `sqrt(x × x + y × y)` |
| aliasing | egri chetning zinapoya bo'lib ko'rinishi |
| `smoothstep` | 0 dan 1 gacha silliq (S shaklida) o'tish |
| `fwidth` | qiymat keyingi pikselga o'tganda qanchaga o'zgarishi |
| blending | yangi rangni ekrandagi rang bilan alpha bo'yicha aralashtirish |

## Keyingi dars

04-darsda zarrachalar jonlanadi: tezlik, gravitatsiya, timestep, devorlar bilan to'qnashuv va har kadrda yangilanadigan VBO.

Shu dars tugagach yozing. Xohlasangiz, kodingizni ko'rib chiqaman.
