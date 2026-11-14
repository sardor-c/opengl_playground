# 01. OpenGL konteksti: GPU bilan birinchi suhbat

> 2026-11-13 da bu dars qadam-baqadam qo'llanma ko'rinishiga o'tkazildi. Endi har bir qadamda avval nazariya beriladi,
> keyin aynan shu nazariyaga tegishli kod va uni qayerga yozish ko'rsatiladi, oxirida dasturni yig'ib tekshirasiz.
> **Yakuniy kod o'zgarmagan.** 01-darsni bajarib bo'lgansiz, loyihangizda hech narsani almashtirish shart emas.
> Har bir oraliq qadamning kodi alohida yig'ilib, ishga tushirib tekshirildi.

## Bu darsda nima qo'shamiz va nega aynan hozir

00-darsda bo'sh oyna ochdik, lekin uning ichini hech kim chizmasdi. Bu darsda dasturimiz birinchi marta videokarta (GPU)
bilan gaplashadi: oynaga **OpenGL konteksti**ni ulaymiz, OpenGL funksiyalarini **yuklaymiz** va oynani har kadrda rangga
bo'yaymiz. Rang sekin o'zgarib turadi, sarlavhada esa soniyasiga necha kadr chizilayotgani (FPS) va oyna o'lchamlari chiqadi.

Uchburchak keyingi darsda. Undan oldin shu poydevor kerak: kontekstsiz birorta `gl...` funksiya ishlamaydi, yuklanmagan
funksiya esa dasturni yiqitadi. Bu darsdan keyin kursdagi har bir `gl...` chaqiruvi shu poydevorga tayanadi.

Oldindan kompyuter grafikasi yoki matematika bilish shart emas. Har bir tushuncha noldan tushuntiriladi: avval kundalik misol,
keyin kichik sonlar, oxirida kodda qayerda turgani. Dars oxirida qisqa lug'at bor.

Dars oxirida loyiha shunday bo'ladi:

```
opengl_fluid_sim_scratch/
├── CMakeLists.txt          (o'zgaradi)
├── external/khronos/       (yangi: Khronos header'lari)
│   ├── GL/glcorearb.h
│   └── KHR/khrplatform.h
└── src/
    ├── gl.hpp              (yangi)
    ├── gl.cpp              (yangi)
    └── main.cpp            (o'zgaradi)
```

## Dars qanday tuzilgan

Dars 8 qadamdan iborat. Har bir qadam uch qismdan tuzilgan:

1. **Nazariya**: bu narsa nima uchun kerak va qanday ishlaydi.
2. **Kod**: aynan shu nazariyaga tegishli qatorlar va ularni qaysi faylning qayeriga yozish.
3. **Yig'ib ko'ring**: dasturni yig'ib ishga tushirasiz va nima ko'rinishi kerakligini tekshirasiz.

Har bir qadamdan keyin dastur yig'iladi va ishlaydi. Biror qadamda kutilgan natija chiqmasa, keyingisiga o'tmang: xato
aynan shu qadamda qo'shilgan bir necha qatorda.

| Qadam | Nima qilamiz | Natija |
|---|---|---|
| 1 | Oynaga OpenGL kontekstini so'raymiz | oyna ochiladi, ichi hali chizilmagan |
| 2 | Birinchi OpenGL funksiyasini yuklaymiz | terminalda OpenGL versiyasi va GPU nomi |
| 3 | Funksiyalar ro'yxatini X-macro bilan tartibga solamiz | natija o'zgarmaydi, kod tartibli bo'ladi |
| 4 | Oynani bo'yaymiz | oyna to'q ko'k |
| 5 | FPS'ni o'lchaymiz va vsync'ni yoqamiz | sarlavhada `165 FPS` |
| 6 | Rangni vaqtga bog'laymiz | rang asta o'zgarib turadi |
| 7 | Oyna va framebuffer o'lchamlarini hisobga olamiz | sarlavhada o'lchamlar |
| 8 | Xatolarni ushlaymiz | xato bo'lsa, terminalda xabar |

**Kod qanday ko'rsatiladi.** Yangi fayl to'liq beriladi. Mavjud faylga kiritiladigan o'zgarish esa **diff** ko'rinishida
ko'rsatiladi. Masalan, 4-qadamda sikl ichidagi bitta qator almashadi:

```diff
     while (!glfwWindowShouldClose(window)) {
-        glfwWaitEvents();
+        glfwPollEvents();
     }
```

- `-` bilan boshlangan qatorni **o'chiring**.
- `+` bilan boshlangan qatorni **qo'shing**.
- Belgisiz qatorlar faylda allaqachon bor. Ular faqat joyni topish uchun ko'rsatilgan, ularga tegmang.
- `+` va `-` belgisining o'zini yozmang, faqat undan keyingi matnni bo'shliqlari bilan birga yozing.

Har bir qadam oxirida o'zgargan faylning to'liq holati yashirin blokda turadi: "▶" belgili qatorni bossangiz ochiladi.
Kodingizni u bilan solishtirishingiz mumkin.

**Yig'ish.** Har bir "Yig'ib ko'ring" bo'limida loyihaning ildizida shu buyruqlar ishlatiladi:

```
cmake --build build
.\build\fluid_sim.exe      # Windows (PowerShell)
./build/fluid_sim          # macOS
```

`CMakeLists.txt` o'zgarsa, CMake o'zini qayta ishga tushiradi, alohida `cmake -S . -B build` kerak emas. Dasturni oynaning
X tugmasi bilan yopasiz.

**Boshlanish nuqtasi** 00-darsning oxiridagi holat. `src/main.cpp` da GLFW oyna ochadi, lekin `GLFW_NO_API` hint'i "bu oynaga
OpenGL kerak emas" deydi, sikl esa `glfwWaitEvents` bilan hodisalarni kutib turadi.

## 1-qadam. Oynaga OpenGL kontekstini so'raymiz

Avval GPU bilan gaplasha oladigan "kanal" kerak. Uni tushunish uchun GPU, drayver va OpenGL nima ekanidan boshlaymiz.

### 1.1. GPU va drayver

Kompyuterda ikkita "hisoblovchi" bor.

**CPU** (protsessor) bir nechta kuchli ishchiga o'xshaydi. Sizning i5-12400F protsessoringizda 6 ta yadro bor: har biri
murakkab ishni tez bajaradi, lekin ular oz.

**GPU** (videokarta) esa minglab oddiy ishchilardan iborat. Har biri sodda, lekin ular bir vaqtda ishlaydi. Uzun devorni
bo'yash kerak deb tasavvur qiling: bitta usta har bir taxtani birma-bir bo'yaguncha, ming nafar o'quvchi har biri bitta
taxtani bo'yab bo'ladi. Rasm chizish xuddi shunday ish: millionlab piksel, har biri uchun bir xil hisob. Shuning uchun uni GPU
qiladi. GPU'ning o'z xotirasi ham bor (VRAM).

**Drayver** (driver) bu GPU ishlab chiqaruvchisi yozgan dastur, u bizning buyruqlarimizni GPU tushunadigan tilga tarjima qiladi.
Windows'da bu NVIDIA drayveri, Mac'da Apple'ning o'zi. Har bir kompyuterda drayver har xil, chunki GPU'lar har xil.

### 1.2. OpenGL nima

OpenGL dastur ham, kutubxona ham emas. U **spetsifikatsiya**: Khronos guruhi yozgan hujjat, unda "`glClear` chaqirilsa,
mana bu bo'lishi kerak" degan qoidalar bor.

Yo'l harakati qoidalarini eslang. Qoidalar hamma uchun bitta: "qizil chiroqda to'xta". Lekin mashinalar har xil, har birini
boshqa zavod yasagan. Ular qoidani har xil mexanizm bilan bajaradi, natija esa bir xil: mashina to'xtaydi. OpenGL ham shunday:
hujjat bitta, uni bajaradigan kodni har bir drayver o'zi yozadi. Bir xil `glClear` Windows'da NVIDIA kodini, Mac'da Apple kodini
ishga tushiradi, natija esa bir xil bo'ladi. Bizning kod ikkala kompyuterda ishlashining sababi shu.

### 1.3. Kontekst

OpenGL'ning hamma sozlamalari va yaratilgan narsalari (keyingi darslardagi buffer, shader, tekstura) bitta obyektda saqlanadi.
Bu obyekt **kontekst** (context) deyiladi. U yana bitta muhim narsani biladi: qaysi oynaga chizish kerak.

Kontekstni ustaxona deb tasavvur qiling: asboblar o'z joyida, boshlangan ishlar stolda. `gl...` funksiyalarining birortasida
"qaysi ustaxonada" degan parametr yo'q. Shuning uchun avval ustaxonaga kirib olish kerak, keyingi har bir buyruq shu yerda
bajariladi.

`glfwMakeContextCurrent(window)` aynan shu ishni qiladi: "shu **oqim** endi shu oyna kontekstida ishlaydi". Oqim (thread) bu
dastur ichidagi bitta bajarish yo'li. Dastur bir vaqtda bir nechta ishni bir nechta oqimda qila oladi, bizda esa hozircha bitta,
`main` ishlayotgan oqim.

### 1.4. Versiya va profil: nega 4.1 core

OpenGL 1992-yildan beri o'sib keladi. Telefoningizdagi tizim kabi, uning yangi versiyalarida yangi imkoniyatlar qo'shiladi:

| versiya | yil | nima qo'shilgan (bizga tegishlisi) |
|---|---|---|
| 1.1 | 1997 | eski asoslar; Windows bilan birga keladigan `opengl32.dll` hanuz shu darajada |
| 3.2 | 2009 | *core profile*: eski uslub alohida ajratildi |
| 4.1 | 2010 | Mac qo'llaydigan oxirgi versiya |
| 4.3 | 2012 | compute shader, qulay xato xabarlari (KHR_debug) |
| 4.6 | 2017 | oxirgi versiya |

**Nega 4.1.** Apple OpenGL'ni 2013-yilda 4.1 da to'xtatgan, 2018-yilda esa eskirgan (deprecated) deb e'lon qilgan. U hali
ishlaydi, lekin yangilanmaydi. Bir xil kod ikkala kompyuterda ishlashi uchun 4.1 bizning shiftimiz: undan yangi narsalarni
ishlatmaymiz, hatto Windows ularni qo'llasa ham.

**Core profil.** 1990-yillardagi eski uslubda har bir uchburchak uchidan bitta-bitta funksiya chaqirilardi (`glBegin`, `glVertex3f`).
Bu sekin edi va 3.2 versiyada *compatibility* (moslik) profiliga ajratildi. *Core* (asosiy) profilda eski uslub yo'q, hammasini
buffer va shader bilan qilamiz (02-dars). Mac'da 3.2 dan yuqori versiyalar faqat core profilda bor.

Qanday kontekst kerakligini GLFW'ga oynani yaratishdan **oldin** "maslahat" (hint) bilan aytamiz:

| Hint | Qiymat | Ma'nosi |
|---|---|---|
| `GLFW_CONTEXT_VERSION_MAJOR`, `MINOR` | 4, 1 | OpenGL 4.1 kerak (4 katta, 1 kichik raqam) |
| `GLFW_OPENGL_PROFILE` | `GLFW_OPENGL_CORE_PROFILE` | eski uslubsiz, core profil |
| `GLFW_OPENGL_FORWARD_COMPAT` | `GLFW_TRUE` | "eskirgan" deb belgilangan hamma narsa ham olib tashlansin |

**Forward-compat** nima uchun kerak? GLFW 3.3 Mac'da bu hintsiz kontekst bermaydi. Uning manba kodida shunday xato matni bor:

```
NSGL: The targeted version of macOS only supports forward-compatible core profile contexts for OpenGL 3.2 and above
```

GLFW 3.4 da bu tekshiruv olib tashlangan, lekin `CMakeLists.txt` GLFW 3.3 ni ham qabul qiladi, shuning uchun bu qator qoladi.
Windows'da hintni olib tashlab ko'rdim: hech narsa o'zgarmadi.

### 1.5. Kod: `src/main.cpp`

Ikki joyni o'zgartiramiz.

**Birinchi joy.** `glfwCreateWindow` dan oldingi `GLFW_NO_API` qatorini o'chirib, o'rniga to'rtta hint yozing:

<!-- diff s0 s1 src/main.cpp -->
```diff
     std::printf("GLFW: %s\n", glfwGetVersionString());
 
-    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
+    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
+    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
+    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
+    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
+
     GLFWwindow* window = glfwCreateWindow(1280, 720, "Fluid Sim", nullptr, nullptr);
```

Hint'lar `glfwCreateWindow` dan **oldin** turishi shart, chunki GLFW ularni oyna yaratilayotgan paytda o'qiydi. So'ralgan
kontekstni berib bo'lmasa, `glfwCreateWindow` `nullptr` qaytaradi, sababini esa 00-darsda yozgan `on_glfw_error` terminalga
chiqaradi.

**Ikkinchi joy.** Oyna yaratilganini tekshiradigan `if` dan keyin kontekstni joriy qiling:

<!-- diff s0 s1 src/main.cpp -->
```diff
     if (!window) {
         glfwTerminate();
         return 1;
     }
 
+    glfwMakeContextCurrent(window);
+
     while (!glfwWindowShouldClose(window)) {
```

`glfwMakeContextCurrent` `if (!window)` tekshiruvidan keyin turadi: oyna yaratilmagan bo'lsa, uning konteksti ham yo'q.

### 1.6. Yig'ib ko'ring

- Oyna avvalgidek ochiladi. Ichi oq yoki boshqa biror rangda bo'lishi mumkin: kontekst bor, lekin hali hech kim chizmayapti,
  shuning uchun u yerda nima turishi aniqlanmagan. Mening kompyuterimda oq bo'ldi.
- Terminalda faqat `GLFW: ...` qatori, xato yo'q.

Kontekst haqiqatan so'ralayotganini tekshirish uchun ataylab imkonsiz versiya so'rab ko'ring:
`GLFW_CONTEXT_VERSION_MAJOR, 4` ni `9` ga o'zgartiring. Dastur oyna ochmasdan `1` kodi bilan chiqadi:

```
GLFW xatosi 65543: WGL: Driver does not support OpenGL version 9.1
```

Ma'nosi: "drayver OpenGL 9.1 ni qo'llamaydi". WGL bu Windows'ning OpenGL kontekstlarini yaratadigan qismi, Mac'da xabar boshqacha
bo'ladi. Sinab bo'lgach, `4` ni qaytaring.

Yana bir tez-tez uchraydigan xato: `GLFW_NO_API` qatorini o'chirish esdan chiqsa, u to'rtta hintdan ustun turadi va oyna
kontekstsiz yaratiladi. Bunda terminalda shunday xabar chiqadi:

```
GLFW xatosi 65546: Cannot make current with a window that has no OpenGL or OpenGL ES context
```

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/main.cpp</code> (solishtirish uchun)</summary>

```cpp
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdio>

static void on_glfw_error(int code, const char* description) {
    std::fprintf(stderr, "GLFW xatosi %d: %s\n", code, description);
}

int main() {
    glfwSetErrorCallback(on_glfw_error);

    if (!glfwInit()) {
        return 1;
    }
    std::printf("GLFW: %s\n", glfwGetVersionString());

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

    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

</details>

## 2-qadam. Birinchi OpenGL funksiyasini yuklaymiz

Kontekst bor. Endi undan birinchi savolni so'raymiz: "sen qaysi OpenGL versiyasisan va qaysi GPU'da ishlayapsan?". Bunga
`glGetString` funksiyasi javob beradi. Lekin uni oddiygina chaqirib bo'lmaydi, avval **yuklash** kerak. Buning sababini tushunish
uchun funksiya chaqiruvi qanday ishlashidan boshlaymiz.

Bu qadamda to'rtta faylga tegamiz, ulardan ikkitasi yangi. Shuning uchun dasturni faqat oxirida, 2.8-bo'limda yig'amiz.

### 2.1. Funksiya qanday chaqiriladi: xotira manzili

**Xotira va manzil.** Kompyuter xotirasini juda uzun ko'chadagi uylar qatori deb tasavvur qiling. Har bir uyda bitta bayt yashaydi
va har bir uyning **raqami** bor. Bu raqam **manzil** (address) deyiladi. Dastur ishga tushganda uning kodi ham xotiraga yuklanadi,
shuning uchun har bir funksiya biror manzildan boshlanadi.

**Funksiyani chaqirish** protsessor uchun "falon manzilga bor va o'sha yerdagi buyruqlarni bajar" degani. Odatda bu manzilni
dasturni yig'ishda linker yozib qo'yadi (00-dars).

**Funksiya ko'rsatkichi** (function pointer) bu funksiya manzilini saqlaydigan o'zgaruvchi. Uni oddiy funksiyadek chaqirish mumkin.
Kichik misol (bu loyihaga kirmaydi, men uni alohida yig'ib ishga tushirdim):

```cpp
#include <cstdio>

static void salom(int n) {
    std::printf("salom, %d\n", n);
}

int main() {
    void (*f)(int) = salom;

    f(5);
    std::printf("salom funksiyasining manzili: %p\n", reinterpret_cast<void*>(f));
    return 0;
}
```

Natija:

```
salom, 5
salom funksiyasining manzili: 00007ff7667e14d0
```

`void (*f)(int)` deb o'qiladi: "`f` ko'rsatkich, u `int` oladigan va hech narsa qaytarmaydigan (`void`) funksiyaga ishora qiladi".
`f = salom` ichiga `salom` ning manzilini yozadi, `f(5)` esa `salom(5)` bilan bir xil ish qiladi. Manzil o'n oltilik sanoq tizimida
yozilgan katta son. Sizda boshqa son chiqadi, bu muhim emas.

### 2.2. Nega OpenGL funksiyalarini yuklash kerak

00-darsda GLFW funksiyalarini linker o'zi uladi, chunki GLFW kutubxonasi fayli bor edi. OpenGL'da bunday qilib bo'lmaydi: haqiqiy
kod drayverda, drayver esa har bir kompyuterda boshqa. Windows bilan birga keladigan `opengl32.dll` faqat juda eski OpenGL 1.1 ni
beradi. Uni sizning kompyuteringizda tekshirdim: ichida 343 ta `gl...` funksiya bor.

| Funksiya | `opengl32.dll` da |
|---|---|
| `glClear`, `glClearColor`, `glViewport`, `glGetString` | bor |
| `glGenBuffers`, `glGenVertexArrays`, `glUseProgram` | yo'q |

Bizga kerak bo'ladigan yuzlab funksiya shu "yo'q" qatorida. Yechim: dastur ishlab turganda drayverdan so'raymiz: "`glGenBuffers`
ning manzili qayerda?". Javobni funksiya ko'rsatkichiga yozib qo'yamiz va keyin uni oddiy funksiyadek chaqiramiz. Bu ish
**funksiyalarni yuklash** deyiladi. So'rovni `glfwGetProcAddress("nom")` bajaradi: GLFW bizning o'rnimizga drayverga murojaat qiladi.

Jadvalning "bor" qatoridagi funksiyalarni ham shu yo'l bilan olamiz. Shunda hamma funksiya uchun bitta usul bo'ladi, Mac'da esa
`opengl32.dll` umuman yo'q. Shuning uchun dasturimiz `opengl32` bilan umuman bog'lanmaydi (link qilinmaydi).

**Tartib qat'iy.** Manzilni faqat kontekst bor paytda so'rash mumkin, chunki aynan kontekst qaysi drayver va qaysi versiya
ekanini biladi:

```
oyna + kontekst yaratish ──► glfwMakeContextCurrent ──► gl_load ──► gl... chaqiruvlari
```

`gl_load` biz yozadigan funksiya bo'ladi: u kerakli hamma manzillarni so'rab oladi. Hozircha unda bitta funksiya, `glGetString`.

### 2.3. Khronos header'lari

Funksiya ko'rsatkichini yaratish uchun uning **turi** kerak: qanday parametrlar oladi va nima qaytaradi. `GLenum`, `GL_VERSION`
kabi nomlar ham kerak. Bularning hammasi Khronos'ning rasmiy header fayli **`GL/glcorearb.h`** da. Unda ikki xil narsa bor:

- **Konstantalar**: nomlangan sonlar. Masalan, `#define GL_VERSION 0x1F02`. Bizga sonning o'zi emas, nomi muhim.
- **Funksiya ko'rsatkichi turlari.** Masalan, `glGetString` uchun:

  ```c
  typedef const GLubyte *(APIENTRYP PFNGLGETSTRINGPROC) (GLenum name);
  ```

  Bu "`PFNGLGETSTRINGPROC` degan tur: bitta `GLenum` oladigan va `const GLubyte*` qaytaradigan funksiyaga ko'rsatkich" degani,
  ya'ni 2.1-bo'limdagi `void (*f)(int)` ning o'zi, faqat nom berilgan. Nom qolipi: `PFN` + `GL` + funksiya nomi katta harflarda
  + `PROC`. `glClear` uchun bu `PFNGLCLEARPROC`, `glViewport` uchun `PFNGLVIEWPORTPROC`.

macOS SDK'da bu fayl yo'q, shuning uchun uni loyihaga qo'shamiz va ikkala kompyuter bir xil faylni ishlatadi.

**Kod.** Loyihaning ildizida (Windows PowerShell'da) ikki faylni yuklab oling:

```
curl.exe -L --create-dirs -o external/khronos/GL/glcorearb.h https://registry.khronos.org/OpenGL/api/GL/glcorearb.h
curl.exe -L --create-dirs -o external/khronos/KHR/khrplatform.h https://registry.khronos.org/EGL/api/KHR/khrplatform.h
```

`curl` fayl yuklab oladigan dastur. `-L` sahifa boshqa manzilga yo'naltirsa, o'sha manzilga ergashadi. `--create-dirs` kerakli
papkalarni yaratadi. `-o` faylni qayerga saqlashni aytadi. `.exe` muhim: PowerShell'da oddiy `curl` boshqa buyruqqa ishora qiladi.
`khrplatform.h` ni `glcorearb.h` o'zi ichida `#include` qiladi, shuning uchun ikkalasi kerak. Bu ikki fayl Git'ga kiradi, Mac
ularni `git pull` bilan oladi.

**Nega glad emas.** glad kabi dasturlar yuklash kodini avtomatik yaratib beradi. Biz uni qo'lda yozamiz: kod atigi bir necha qator,
mexanizm ko'zga ko'rinib turadi, ro'yxatda faqat ishlatilgan funksiyalar bo'ladi va har bir yangi OpenGL funksiyasi loyihaga
ongli ravishda kiradi.

### 2.4. Kod: `CMakeLists.txt`

Ikki narsani qo'shamiz: yangi `src/gl.cpp` faylini dastur tarkibiga va header'lar papkasini compiler'ning qidiruv yo'liga:

<!-- diff s1 s2 CMakeLists.txt -->
```diff
 add_executable(fluid_sim
     src/main.cpp
+    src/gl.cpp
 )
 
+target_include_directories(fluid_sim PRIVATE external/khronos)
 target_compile_options(fluid_sim PRIVATE -Wall -Wextra)
```

`target_include_directories` compiler'ga `#include <GL/glcorearb.h>` ni `external/khronos` papkasidan qidirishni aytadi.
`gl.cpp` ro'yxatga kirmasa, u kompilyatsiya qilinmaydi va linker `gl_load` ni topa olmaydi: menyuda bor, oshxonada yo'q
(00-dars). `gl.cpp` hali yo'q, shuning uchun hozir yig'mang: CMake mavjud bo'lmagan faylni ko'rib xato beradi.

### 2.5. Kod: `src/gl.hpp` (yangi fayl)

Har bir OpenGL funksiyasi uchun ikki narsa yozishimiz kerak:

1. **E'lon** (declaration), header'da: "shunday nomli ko'rsatkich bor".
2. **Ta'rif** (definition), bitta `.cpp` da: ko'rsatkichning o'zi, ya'ni xotiradagi joyi.

E'lon bilan ta'rif farqini telefon kitobi bilan tushunish mumkin. E'lon kitobdagi yozuvga o'xshaydi: "Aliyevlar oilasi shu shaharda
yashaydi". Ta'rif esa uyning o'zi. Kitobda yozuv ko'p bo'lishi mumkin (header'ni har bir `.cpp` o'qiydi), uy esa bitta bo'lishi
shart. C++ da e'lonni `extern` so'zi bildiradi: "bu o'zgaruvchi bor, lekin u boshqa joyda yaratilgan".

Ko'rsatkichni aynan funksiya nomi bilan, `glGetString` deb ataymiz. Shunda qolgan kod oddiy OpenGL kabi o'qiladi:
`glGetString(GL_VERSION)`.

`src` papkasida yangi `gl.hpp` faylini yarating:

<!-- file s2 src/gl.hpp -->
```cpp
#pragma once

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif
#include <GL/glcorearb.h>

extern PFNGLGETSTRINGPROC glGetString;

bool gl_load();
```

**`#pragma once`** header bitta `.cpp` ga ikki marta kirib qolsa ham, uni faqat bir marta o'qishni aytadi.

**`APIENTRY` bloki.** Funksiya chaqiruvchi va funksiyaning o'zi "parametrlarni qayerga qo'yamiz" degan masalada kelishib olishi kerak.
Ikki kishi "kalitni gilam tagiga qo'yaman" deb kelishganiga o'xshaydi. Bu **chaqirish kelishuvi** (calling convention) deyiladi.
`glcorearb.h` uni `APIENTRY` makrosidan oladi (makro nima ekanini 3-qadamda ko'ramiz). Windows'da bu makro aniqlanmagan bo'lsa,
header butun `<windows.h>` ni qo'shib oladi. Bu zararli: `<windows.h>` `near` va `far` degan bo'sh makrolar yaratadi. Bu blokni olib
tashlab, `main` ichiga `float near = 0.1f;` yozib ko'rdim, preprocessor `near` ni o'chirib yubordi va compiler shunday dedi:

```
error: expected unqualified-id before '=' token
```

Kamera darsida aynan `near` va `far` degan nomlar kerak bo'ladi. `#if defined(_WIN32)` faqat Windows'da rost bo'ladi, shuning uchun bu
blok Mac'ga ta'sir qilmaydi. Loyihadagi kam uchraydigan platformaga xos joylardan biri.

**`extern PFNGLGETSTRINGPROC glGetString;`** e'lon: "`glGetString` degan, `PFNGLGETSTRINGPROC` turidagi ko'rsatkich bor".

**`bool gl_load();`** yuklash funksiyasining e'loni. `bool` "rost yoki yolg'on" qaytaradi degani: hamma manzil topilsa `true`.

### 2.6. Kod: `src/gl.cpp` (yangi fayl)

Endi ta'rif va yuklashning o'zi. `src` papkasida yangi `gl.cpp` faylini yarating:

<!-- file s2 src/gl.cpp -->
```cpp
#include "gl.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdio>

PFNGLGETSTRINGPROC glGetString = nullptr;

bool gl_load() {
    glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(glfwGetProcAddress("glGetString"));
    if (!glGetString) {
        std::fprintf(stderr, "OpenGL funksiyasi topilmadi: glGetString\n");
        return false;
    }
    return true;
}
```

**`PFNGLGETSTRINGPROC glGetString = nullptr;`** ta'rif: ko'rsatkichning o'zi shu yerda yaratiladi va butun dasturda bitta bo'ladi.
`nullptr` "hali hech qayerga ko'rsatmaydi" degani.

**`gl_load`** uch ish qiladi:

1. `glfwGetProcAddress("glGetString")` drayverdan manzilni so'raydi.
2. U "qandaydir funksiya manzili" qaytaradi. **`reinterpret_cast<PFNGLGETSTRINGPROC>`** compiler'ga "shu manzilni aynan shu turdagi
   ko'rsatkich deb qabul qil" deydi. Bu `cast`, ya'ni turni o'zgartirish. `reinterpret` so'zi "baytlarga boshqacha qara" degan
   ma'noni beradi.
3. Manzil topilmasa (`!glGetString`, ya'ni ko'rsatkich hali `nullptr`), funksiya nomini aytamiz va `false` qaytaramiz. Xato nomsiz
   crash bo'lib chiqqandan ko'ra shunisi ancha yaxshi.

Xabar `stderr` ga, ya'ni xatolar uchun ajratilgan oqimga yoziladi. Terminalda u oddiy matn bilan birga ko'rinadi.

Windows'da GLFW yangi funksiyalarni drayverdan, 1.1 funksiyalarini esa `opengl32.dll` dan oladi. Biz bu farqni ko'rmaymiz.

### 2.7. Kod: `src/main.cpp`

To'rtta joyni o'zgartiramiz.

**Faylning eng boshiga** `gl.hpp` ni qo'shing:

<!-- diff s1 s2 src/main.cpp -->
```diff
+#include "gl.hpp"
+
 #define GLFW_INCLUDE_NONE
 #include <GLFW/glfw3.h>
```

OpenGL turlari (`GLenum`), konstantalar (`GL_VERSION`) va bizning ko'rsatkichimiz shu fayldan keladi.

**`on_glfw_error` funksiyasidan keyin** kichik yordamchi funksiya qo'shing:

<!-- diff s1 s2 src/main.cpp -->
```diff
 static void on_glfw_error(int code, const char* description) {
     std::fprintf(stderr, "GLFW xatosi %d: %s\n", code, description);
 }
 
+static const char* gl_string(GLenum name) {
+    return reinterpret_cast<const char*>(glGetString(name));
+}
+
 int main() {
```

`glGetString` OpenGL haqida matn qaytaradi, lekin uning turi `const GLubyte*` (ishorasiz baytlar), `printf` esa `const char*` kutadi.
`reinterpret_cast` bu farqni yopadi: baytlar bir xil, faqat turning nomi boshqa.

**GLFW qatoriga** ikkita bo'shliq qo'shing, shunda keyingi qatorlar bilan bir ustunda turadi:

<!-- diff s1 s2 src/main.cpp -->
```diff
-    std::printf("GLFW: %s\n", glfwGetVersionString());
+    std::printf("GLFW:   %s\n", glfwGetVersionString());
```

**`glfwMakeContextCurrent` dan keyin** funksiyalarni yuklang va kontekstdan javob so'rang:

<!-- diff s1 s2 src/main.cpp -->
```diff
     glfwMakeContextCurrent(window);
+    if (!gl_load()) {
+        glfwDestroyWindow(window);
+        glfwTerminate();
+        return 1;
+    }
+
+    std::printf("OpenGL: %s\n", gl_string(GL_VERSION));
+    std::printf("GLSL:   %s\n", gl_string(GL_SHADING_LANGUAGE_VERSION));
+    std::printf("GPU:    %s\n", gl_string(GL_RENDERER));
 
     while (!glfwWindowShouldClose(window)) {
```

`gl_load` aynan `glfwMakeContextCurrent` dan keyin turadi (2.2-bo'limdagi tartib). U muvaffaqiyatsiz bo'lsa, oynani yopib, `1` kodi
bilan chiqamiz. Uch qator kontekst haqida aytadi: OpenGL versiyasi, shader tili (GLSL, 02-dars) versiyasi va GPU nomi. Kelajakda
muammo chiqsa, birinchi qaraladigan joy shu.

### 2.8. Yig'ib ko'ring

Windows'da terminalda shular chiqadi (oyna ichi hali ham chizilmagan):

```
GLFW:   3.5.1 Win32 WGL Null EGL OSMesa MinGW-w64 DLL
OpenGL: 4.1.0 NVIDIA 610.47
GLSL:   4.10 NVIDIA via Cg compiler
GPU:    NVIDIA GeForce RTX 3060/PCIe/SSE2
```

Drayver aynan biz so'ragan 4.1 kontekstni berdi. Mac'da `OpenGL:` qatori `4.1 Metal - ...` ko'rinishida bo'lishi kutiladi.

**Nima buzilsa nima bo'ladi.** Hammasini shu qadamning kodida sinab ko'rdim:

| O'zgarish | Natija |
|---|---|
| `gl_load` `glfwMakeContextCurrent` dan **oldin** | dastur `1` kodi bilan chiqadi, ikki qator xabar (pastda) |
| `gl_load` umuman chaqirilmasa | `OpenGL:` qatorlari chiqmaydi, oyna oq qoladi, dastur hech qanday xabarsiz yiqiladi (Windows'da chiqish kodi `0xC0000005`, ya'ni "xotiraga noto'g'ri murojaat") |
| `#include "gl.hpp"` yo'q | kompilyatsiya xatosi: `'GLenum' was not declared in this scope`, keyin `'gl_load' was not declared in this scope` |
| `CMakeLists.txt` da `src/gl.cpp` yo'q | linker xatosi: ``undefined reference to `gl_load()'`` |
| `target_include_directories` yo'q | Windows'da **yig'iladi** (pastda sababi), Mac'da `'GL/glcorearb.h' file not found` xatosi kutiladi |

Birinchi qatordagi xabar:

```
GLFW xatosi 65538: Cannot query entry point without a current OpenGL or OpenGL ES context
OpenGL funksiyasi topilmadi: glGetString
```

"Joriy kontekst yo'q, manzilni so'rab bo'lmaydi". Ikkinchi qator esa bizning `gl_load` xabarimiz.

Ikkinchi holat ayniqsa yomon: bo'sh ko'rsatkich "0-uyga bor" degani, u yerda esa hech qanday funksiya yo'q.

Oxirgi qator kutilmagan natija. MSYS2'ning `mingw-w64-x86_64-headers` paketida o'zining `GL/glcorearb.h` fayli bor, compiler qidiruv
yo'li ko'rsatilmasa o'shani topib oladi. Windows'da xato ko'rinmaydi, Mac'da esa bunday fayl yo'q. Mac'ni buzadigan xatoni Windows
yashiradigan holatlar kursda yana uchraydi.

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

    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

</details>

## 3-qadam. Bitta ro'yxat: X-macro

Bu qadamda dasturning ishlashi o'zgarmaydi. Faqat kodni shunday qayta tuzamiz, keyingi qadamlarda yangi funksiya qo'shish bitta
qator bo'lsin. Xatti-harakatni o'zgartirmasdan kodni qayta tuzish **refactoring** deyiladi.

### 3.1. Muammo: har bir funksiya uch joyda

`glGetString` ni ishlatish uchun uni uch joyda yozdik:

1. `gl.hpp` da e'lon: `extern PFNGLGETSTRINGPROC glGetString;`
2. `gl.cpp` da ta'rif: `PFNGLGETSTRINGPROC glGetString = nullptr;`
3. `gl_load` ichida yuklash: `glfwGetProcAddress` bilan besh qator.

Bu darsning o'zida yana to'rtta funksiya qo'shiladi, kurs oxirigacha esa o'nlab. Har biri uchun ikki faylda uch joyni o'zgartirish
kerak bo'lsa, ulardan birini unutish oson. Ta'rif unutilsa, linker xato beradi, bu hali yaxshi. Yuklash unutilsa esa ko'rsatkich
`nullptr` qoladi va dastur 2.8-bo'limdagidek xabarsiz yiqiladi.

Yechim: ro'yxat **bitta** bo'ladi, uch xil kodni esa undan **preprocessor** yasaydi.

### 3.2. Preprocessor va makro

00-darsdan eslang: kompilyatsiyadan oldin preprocessor matnni "topib almashtiradi". `#define` bilan **makro** yaratiladi: nom va
uning o'rniga qo'yiladigan matn. Makro parametr ham olishi mumkin, xuddi funksiya kabi, faqat u kodni bajarmaydi, matnni
almashtiradi. Masalan:

```cpp
#define KVADRAT(a) ((a) * (a))
```

Shundan keyin `KVADRAT(5)` yozilgan joyga preprocessor `((5) * (5))` matnini qo'yadi, compiler esa faqat shu matnni ko'radi.

### 3.3. X-macro

Ro'yxatimiz shunday ko'rinadi:

```cpp
#define GL_FUNCTIONS(X)                  \
    X(PFNGLGETSTRINGPROC, glGetString)
```

Bu "ro'yxatdagi har bir funksiya uchun `X(tur, nom)` ni yoz" degani, `X` esa hali hech narsa emas. Muhrni tasavvur qiling: ro'yxat
muhr bosiladigan qog'ozlar, `X` esa muhrning naqshi. Naqshni almashtirsak, xuddi shu ro'yxatdan boshqa matn chiqadi. `X` ni uch xil
belgilab, uch xil kod olamiz. Bu qadamdagi kodning preprocessor chiqishidan (`g++ -E`) `glGetString` qatorlari:

```cpp
extern PFNGLGETSTRINGPROC glGetString;          // gl.hpp: e'lon
PFNGLGETSTRINGPROC glGetString = nullptr;       // gl.cpp: ta'rif

// gl_load ichida: yuklash (preprocessor buni bitta uzun qatorga yozadi, o'qishga qulay bo'lsin deb qatorlarga bo'ldim)
glGetString = reinterpret_cast<PFNGLGETSTRINGPROC>(glfwGetProcAddress("glGetString"));
if (!glGetString) {
    std::fprintf(stderr, "OpenGL funksiyasi topilmadi: %s\n", "glGetString");
    return false;
}
```

Bu 2-qadamda qo'lda yozgan kodimizning aynan o'zi. Farqi shuki, endi uni preprocessor yozadi va ro'yxatdagi har bir funksiya uchun
takrorlaydi.

### 3.4. Kod: `src/gl.hpp`

`extern` qatorini ro'yxat va uni e'lonlarga ochadigan uch qator bilan almashtiring:

<!-- diff s2 s3 src/gl.hpp -->
```diff
 #include <GL/glcorearb.h>
 
-extern PFNGLGETSTRINGPROC glGetString;
+#define GL_FUNCTIONS(X)                  \
+    X(PFNGLGETSTRINGPROC, glGetString)
+
+#define X(type, name) extern type name;
+GL_FUNCTIONS(X)
+#undef X
 
 bool gl_load();
```

**`\`** qator oxirida "makro keyingi qatorda davom etadi" degani. Shuning uchun ro'yxatning oxirgi qatorida u yo'q: ro'yxat shu
yerda tugaydi. Yangi funksiya qo'shganda oldingi oxirgi qatorga `\` qo'shiladi (4-qadamda shunday qilamiz). `\` lar bir ustunda
tursa, o'qish oson, lekin bu shart emas.

**`#define X(type, name) extern type name;`** muhrning birinchi naqshi: har bir `X(tur, nom)` dan `extern tur nom;` e'loni chiqadi.

**`GL_FUNCTIONS(X)`** ro'yxatni shu naqsh bilan ochadi.

**`#undef X`** `X` nomini o'chiradi, shunda keyingi joyda (`gl.cpp` da) uni yangidan belgilash mumkin.

### 3.5. Kod: `src/gl.cpp` (to'liq almashtiring)

<!-- file s3 src/gl.cpp -->
```cpp
#include "gl.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdio>

#define X(type, name) type name = nullptr;
GL_FUNCTIONS(X)
#undef X

bool gl_load() {
#define X(type, name)                                                     \
    name = reinterpret_cast<type>(glfwGetProcAddress(#name));             \
    if (!name) {                                                          \
        std::fprintf(stderr, "OpenGL funksiyasi topilmadi: %s\n", #name); \
        return false;                                                     \
    }
    GL_FUNCTIONS(X)
#undef X
    return true;
}
```

**Birinchi `#define X`** ikkinchi naqsh: ta'rif. Har bir funksiya uchun `tur nom = nullptr;` chiqadi.

**`gl_load` ichidagi `#define X`** uchinchi naqsh: yuklash. U bir necha qatorli makro, shuning uchun har bir qatori `\` bilan
tugaydi. Ichida bitta yangi belgi bor: **`#name`** parametrni qo'shtirnoqli matnga aylantiradi, `glGetString` dan `"glGetString"`
hosil bo'ladi. Drayverdan nom bo'yicha so'raganda aynan shu matn kerak. 2-qadamda bu matnni qo'lda yozgan edik.

### 3.6. Yig'ib ko'ring

Terminalga 2.8-bo'limdagi to'rt qatorning aynan o'zi chiqadi, oyna ham o'zgarmaydi. Refactoring muvaffaqiyatli bo'lganining belgisi
shu: kod boshqacha, natija bir xil.

**Ro'yxatga faqat OpenGL 4.1 dagi funksiyalar kiradi.** Windows'dagi drayver yangi funksiyalarni yashirmaydi. Ro'yxatga OpenGL 4.3 dagi
`glDispatchCompute` ni qo'shib ko'rdim: Windows'da u topildi, Mac'da esa bunday funksiya yo'q va `gl_load` `false` qaytarishi kerak.
Demak 4.1 chegarasidan chiqib ketsangiz, Windows sizni ogohlantirmaydi. Bu chegarani o'zimiz ushlab turamiz.

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/gl.hpp</code> (solishtirish uchun)</summary>

```cpp
#pragma once

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif
#include <GL/glcorearb.h>

#define GL_FUNCTIONS(X)                  \
    X(PFNGLGETSTRINGPROC, glGetString)

#define X(type, name) extern type name;
GL_FUNCTIONS(X)
#undef X

bool gl_load();
```

</details>

## 4-qadam. Oynani bo'yaymiz

Endi oynaga birinchi marta chizamiz: uni bir xil rang bilan to'ldiramiz. Buning uchun rang qanday saqlanishi, OpenGL buyruqlari
qanday ishlashi va tayyor rasm ekranga qanday chiqishini bilishimiz kerak.

### 4.1. Ekran, piksel va rang

Monitorga juda yaqindan, lupa bilan qarasangiz, rasm mayda nuqtalardan tashkil topganini ko'rasiz. Har bir nuqta **piksel**
(pixel, "picture element", "rasm bo'lagi") deyiladi. 1280 × 720 o'lchamli oynada `1280 × 720 = 921 600` ta piksel bor.

**Rang uchta sondan iborat.** Har bir piksel aslida uchta kichik chiroq: qizil, yashil va ko'k. Ularning yorqinligini
aralashtirib, istalgan rangni hosil qilish mumkin. Bu **RGB** deyiladi (Red, Green, Blue). OpenGL'da har bir yorqinlik
0 dan 1 gacha son:

| qizil, yashil, ko'k | rang |
|---|---|
| (0, 0, 0) | qora (hamma chiroq o'chiq) |
| (1, 1, 1) | oq (hammasi to'liq yonadi) |
| (1, 0, 0) | qizil |
| (1, 1, 0) | sariq (qizil + yashil) |
| (0.05, 0.15, 0.35) | to'q ko'k (bu qadamdagi rang) |

Monitor ichkarida har bir rangni 0 dan 255 gacha butun son bilan saqlaydi, shuning uchun `0.35` aslida `0.35 × 255 = 89.25`, ya'ni
89 bo'ladi. Ekrandan olingan rasmni tekshirganda aynan shunday sonlarni ko'ramiz.

To'rtinchi son **alpha**, ya'ni shaffoflik: 1 to'liq ko'rinadi, 0 umuman ko'rinmaydi. Bu darsda u doim 1. 03-darsda
zarrachalar chetini silliq qilishda ishlatamiz.

**Framebuffer** bu xotiradagi rasm: har bir piksel uchun to'rtta son. 1280 × 720 oynada bu `921 600 × 4 = 3 686 400` bayt,
taxminan 3.7 MB. GPU shu rasmga chizadi, monitor esa uni ekranga chiqaradi. Framebuffer GPU xotirasida turadi.

### 4.2. Holat mashinasi: `glClearColor` va `glClear`

OpenGL'dagi ko'p chaqiruvlar hech narsa chizmaydi, faqat sozlamani o'zgartiradi. Keyingi buyruqlar shu sozlamalardan foydalanadi.
Bunday tizim **holat mashinasi** (state machine) deyiladi. Bo'yoqchini tasavvur qiling: u avval mo'yqalamini bo'yoqqa botiradi
("rang shu"), keyin devorni bo'yaydi. Mo'yqalamdagi rang boshqasiga botirilmaguncha o'zgarmaydi. Bu qadamda:

- `glClearColor(qizil, yashil, ko'k, alpha)` faqat "tozalash rangi shu" deb eslab qoladi, hech narsa chizmaydi;
- `glClear(GL_COLOR_BUFFER_BIT)` esa butun rasmni o'sha eslab qolingan rang bilan bo'yaydi. `GL_COLOR_BUFFER_BIT` "rang qismini
  tozala" degani. Keyinroq framebuffer'ga chuqurlik (depth) qismi ham qo'shiladi va u alohida tozalanadi.

### 4.3. Ikki buffer

Kontekst bilan birga oynaga ikkita framebuffer beriladi. **Front buffer** ekranda ko'rinib turadi, **back buffer**ga esa chizamiz.
Chizish tugagach, `glfwSwapBuffers` ularni almashtiradi: back buffer ekranga chiqadi, eskisi esa navbatdagi chizish uchun bo'shaydi.
Rassom ikkita kanvas bilan ishlayotganini tasavvur qiling: tomoshabin faqat tayyor rasmni ko'radi, keyingisi orqada chiziladi.
Bitta buffer bo'lsa, foydalanuvchi yarim chizilgan rasmni ko'rib qolardi.

### 4.4. Asosiy sikl va hodisalar

**Asosiy sikl** (main loop) dastur yopilguncha aylanib turadigan `while`. Endi har aylanishda uch ish bo'ladi:

```
hodisalarni o'qish ──► rasmni chizish ──► rasmni ko'rsatish ──► (qaytadan)
```

Har bir aylanish bitta **kadr** (frame) beradi. Multfilm ham shunday: tez almashadigan alohida rasmlar.

**Hodisalar.** 00-darsda ko'rdik: tugma bosish, sichqoncha, X tugmasi navbatga tushadi. U yerda `glfwWaitEvents` ishlatgan edik, u
hodisa kelguncha uxlaydi. Endi esa har kadrda chizamiz, shuning uchun `glfwPollEvents` ga o'tamiz: u navbatni tekshiradi va darhol
qaytadi, hodisa bo'lmasa ham.

### 4.5. Kod

**`src/gl.hpp`.** Ro'yxatga ikkita funksiya qo'shing. `glGetString` qatori endi oxirgi emas, shuning uchun uning oxiriga `\` qo'shiladi:

<!-- diff s3 s4 src/gl.hpp -->
```diff
 #define GL_FUNCTIONS(X)                  \
-    X(PFNGLGETSTRINGPROC, glGetString)
+    X(PFNGLGETSTRINGPROC, glGetString)   \
+    X(PFNGLCLEARCOLORPROC, glClearColor) \
+    X(PFNGLCLEARPROC, glClear)
```

`gl.cpp` ga tegmaymiz: X-macro ikki yangi funksiyani o'zi ta'riflaydi va yuklaydi. 3-qadamdagi ishning foydasi shu.

**`src/main.cpp`.** Sikl ichini almashtiring:

<!-- diff s3 s4 src/main.cpp -->
```diff
     while (!glfwWindowShouldClose(window)) {
-        glfwWaitEvents();
+        glfwPollEvents();
+
+        glClearColor(0.05f, 0.15f, 0.35f, 1.0f);
+        glClear(GL_COLOR_BUFFER_BIT);
+
+        glfwSwapBuffers(window);
     }
```

Tartib 4.4-bo'limdagi uch ish: hodisalar, chizish (rangni tanlash va tozalash), ko'rsatish. Sonlar oxiridagi `f` C++ ga "bu `float`
son" deydi: `glClearColor` aynan `float` oladi.

### 4.6. Yig'ib ko'ring

Oyna to'q ko'k rangga bo'yaladi. Oyna markazidagi pikselni ekrandan o'lchadim: `(13, 38, 89)`. Qo'lda hisoblaymiz:
`0.05 × 255 = 12.75`, ya'ni 13; `0.15 × 255 = 38.25`, ya'ni 38; `0.35 × 255 = 89.25`, ya'ni 89. Aynan shu.

Sinab ko'ring: `glfwSwapBuffers(window);` qatorini vaqtincha o'chirsangiz, oyna oq qoladi. GPU har kadrda back buffer'ni bo'yaydi,
lekin u hech qachon ekranga chiqmaydi. Sinab bo'lgach, qatorni qaytaring.

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/gl.hpp</code> (solishtirish uchun)</summary>

```cpp
#pragma once

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif
#include <GL/glcorearb.h>

#define GL_FUNCTIONS(X)                  \
    X(PFNGLGETSTRINGPROC, glGetString)   \
    X(PFNGLCLEARCOLORPROC, glClearColor) \
    X(PFNGLCLEARPROC, glClear)

#define X(type, name) extern type name;
GL_FUNCTIONS(X)
#undef X

bool gl_load();
```

</details>

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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.05f, 0.15f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

</details>

## 5-qadam. FPS va vsync

Oyna bo'yalyapti, lekin soniyasiga necha marta? Buni bilish uchun kadrlarni sanaymiz va natijani sarlavhaga yozamiz. Keyin
sikl monitordan tez aylanmasligi uchun vsync'ni yoqamiz.

### 5.1. Vaqt va FPS

**Vaqt.** `glfwGetTime()` sekundomerga o'xshaydi: GLFW ishga tushganidan beri necha soniya o'tganini beradi, masalan 2.4813.

**FPS** (frames per second) "soniyasiga necha kadr". Kadrlarni sanab boramiz va har 0.5 soniyada `kadrlar soni / o'tgan vaqt` ni
hisoblaymiz. Masalan, 0.503 soniyada 83 kadr chizilgan bo'lsa, `83 / 0.503 = 165` FPS. Bu kurs davomida eng ko'p ishlatiladigan
o'lchov asbobimiz.

Buning uchun ikkita o'zgaruvchi kerak: `fps_time` (oxirgi o'lchov qachon bo'lgani) va `fps_frames` (o'shandan beri nechta kadr
chizilgani).

### 5.2. Kod: `src/main.cpp`

Uch joyga qo'shamiz.

**Sikldan oldin**, `GPU:` qatoridan keyin, ikki o'zgaruvchini yarating:

<!-- diff s4 s5a src/main.cpp -->
```diff
     std::printf("GPU:    %s\n", gl_string(GL_RENDERER));
 
+    double fps_time = glfwGetTime();
+    int fps_frames = 0;
+
     while (!glfwWindowShouldClose(window)) {
```

**Sikl boshida**, `glfwPollEvents` dan keyin, joriy vaqtni oling:

<!-- diff s4 s5a src/main.cpp -->
```diff
         glfwPollEvents();
 
+        const double now = glfwGetTime();
         glClearColor(0.05f, 0.15f, 0.35f, 1.0f);
```

**Sikl oxirida**, `glfwSwapBuffers` dan keyin, kadrni sanang va har 0.5 soniyada sarlavhani yangilang:

<!-- diff s4 s5a src/main.cpp -->
```diff
         glfwSwapBuffers(window);
+
+        fps_frames++;
+        if (now - fps_time >= 0.5) {
+            char title[128];
+            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS",
+                          fps_frames / (now - fps_time));
+            glfwSetWindowTitle(window, title);
+
+            fps_time = now;
+            fps_frames = 0;
+        }
     }
```

Qatorma-qator:

- `const double now` kadr boshidagi vaqt. `const` "bu qiymat kadr davomida o'zgarmaydi" degani. 6-qadamda rang ham shu vaqtdan
  hisoblanadi.
- `fps_frames++` sanagichni bittaga oshiradi: bitta kadr chizildi.
- `if (now - fps_time >= 0.5)`: oxirgi o'lchovdan beri yarim soniya o'tdimi?
- `char title[128]` 128 ta harfga joy (massiv). `std::snprintf` matnni shu massivga yozadi va `sizeof(title)`, ya'ni 128 baytdan
  oshib ketmaydi. `%.0f` "kasrsiz son" degani: 165.3 o'rniga 165 chiqadi.
- `glfwSetWindowTitle` oyna sarlavhasini almashtiradi.
- Oxirgi ikki qator yangi o'lchovni boshlaydi: vaqt hozirgi, sanagich nol.

### 5.3. Yig'ib ko'ring

Sarlavhada FPS chiqadi. Mening kompyuterimda `Fluid Sim | 165 FPS`, ya'ni monitor chastotasi. Nega aynan shu son ekanini keyingi
bo'lim tushuntiradi. Lekin bu natija kafolatlanmagan: biz hali hech narsa sozlamadik, Windows'dagi drayver o'zi shunday tanlagan.
Boshqa drayver yoki Mac boshqa son ko'rsatishi mumkin.

### 5.4. Monitorning yangilanishi, tearing va vsync

**Monitorning yangilanishi.** Monitor rasmni soniyasiga ma'lum marta, yuqoridan pastga qator-qator yangilaydi. Bu son **Hz** (gers)
bilan o'lchanadi: "soniyasiga necha marta". Sizning monitoringiz 165 Hz, ya'ni har `1 / 165 = 0.00606` soniyada (6 millisekund)
yangi rasm. Ko'p Mac ekranlari 60 Hz: `1 / 60 = 0.0167` soniya.

**Tearing va vsync.** Agar bufferlar almashishi monitor rasmni yangilayotgan paytga to'g'ri kelsa, ekranning yuqori qismida eski rasm,
pastki qismida yangisi ko'rinadi, o'rtasida esa "yirtiq" chiziq. Bu **tearing** deyiladi. **Vsync** (vertical synchronization)
"monitor navbatdagi yangilanishni boshlaguncha kut, keyin almashtir" degani. Shunda sikl monitordan tez aylanmaydi: har kadrda
`glfwSwapBuffers` monitorni kutib turadi.

`glfwSwapInterval(n)` buni aniq belgilaydi: "har bir almashtirishdan oldin monitorning `n` ta yangilanishini kut". 1 bo'lsa, har
yangilanishda bitta kadr. 0 bo'lsa, umuman kutmaydi.

### 5.5. Kod: `src/main.cpp`

Drayverning o'z tanloviga ishonmasdan, vsync'ni aniq yoqamiz. `GPU:` qatoridan keyin qo'shing:

<!-- diff s5a s5 src/main.cpp -->
```diff
     std::printf("GPU:    %s\n", gl_string(GL_RENDERER));
 
+    glfwSwapInterval(1);
+
     double fps_time = glfwGetTime();
```

`glfwSwapInterval` joriy kontekstga qo'llanadi, shuning uchun `glfwMakeContextCurrent` dan keyin turishi shart.

### 5.6. Yig'ib ko'ring

Sarlavhada yana `165 FPS`, endi esa bu biz so'ragan natija. Turli qiymatlarni shu qadamning kodida sinab ko'rdim:

| Variant | FPS | Oyna |
|---|---|---|
| `glfwSwapInterval(1)` | 165 (monitor chastotasi) | to'q ko'k |
| `glfwSwapInterval(2)` | 82 (har ikkinchi yangilanishda: `165 / 2`) | to'q ko'k |
| `glfwSwapInterval(0)` | ~5 100 | to'q ko'k |
| `glfwSwapBuffers` yo'q | ~495 000 | oq: hamma kadr back buffer'da qolib ketdi |

Vsync'siz dastur soniyasiga minglab kadr chizadi, monitor esa baribir 165 tasini ko'rsata oladi. Qolgani behuda ish va GPU'ning
qizishi. Vsync'siz son kompyuterga va kodning mayda farqlariga bog'liq: dars oxiridagi kodda ~9 700 chiqdi. Muhimi, u minglab.

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

        const double now = glfwGetTime();
        glClearColor(0.05f, 0.15f, 0.35f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        fps_frames++;
        if (now - fps_time >= 0.5) {
            char title[128];
            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS",
                          fps_frames / (now - fps_time));
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

## 6-qadam. Rang o'zgarib tursin

Agar oyna bir xil rangda tursa, dastur ishlayaptimi yoki qotib qolganmi, bilib bo'lmaydi. Shuning uchun rangni vaqtga bog'laymiz:
u sekin to'q ko'kdan och ko'kka va qaytib o'zgarib turadi.

### 6.1. Sinus

Buning uchun **sinus** (`sin`) kerak. Hozircha uning bitta xossasini bilish yetarli: vaqt o'tgan sari `sin(vaqt)` belanchak kabi
silliq tebranadi, −1 dan 1 gacha borib-keladi va har 6.28 soniyada takrorlanadi:

| vaqt, soniya | 0 | 0.785 | 1.571 | 3.142 | 4.712 | 6.283 |
|---|---|---|---|---|---|---|
| `sin(vaqt)` | 0 | 0.707 | 1 | 0 | −1 | 0 |

Rang uchun bizga 0 dan 1 gacha son kerak, shuning uchun `wave = 0.5 + 0.5 × sin` qilamiz: −1 dan 0 chiqadi, 0 dan 0.5, 1 dan 1.
Keyin yashil va ko'k kanallarni `wave` ga bog'laymiz:

| `wave` | qizil | yashil | ko'k | 0..255 da |
|---|---|---|---|---|
| 0 | 0.05 | 0.15 | 0.35 | (13, 38, 89) to'q ko'k, 4-qadamdagi rang |
| 0.5 | 0.05 | 0.275 | 0.575 | (13, 70, 147) |
| 1 | 0.05 | 0.40 | 0.80 | (13, 102, 204) och ko'k |

Yashil `0.15 + 0.25 × wave`: `wave` 0 bo'lsa 0.15, 1 bo'lsa `0.15 + 0.25 = 0.40`. Ko'k `0.35 + 0.45 × wave`: 0.35 dan 0.80 gacha.

### 6.2. Kod: `src/main.cpp`

**`<cstdio>` dan oldin** `<cmath>` ni qo'shing, `std::sin` shu fayldan keladi:

<!-- diff s5 s6 src/main.cpp -->
```diff
 #include <GLFW/glfw3.h>
 
+#include <cmath>
 #include <cstdio>
```

**Sikl ichida** o'zgarmas rang o'rniga `wave` dan hisoblanadigan rangni yozing:

<!-- diff s5 s6 src/main.cpp -->
```diff
         const double now = glfwGetTime();
-        glClearColor(0.05f, 0.15f, 0.35f, 1.0f);
+        const float wave = 0.5f + 0.5f * static_cast<float>(std::sin(now));
+        glClearColor(0.05f, 0.15f + 0.25f * wave, 0.35f + 0.45f * wave, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT);
```

`now` `double` turida (katta aniqlikdagi son), `glClearColor` esa `float` oladi. `static_cast<float>` `double` sonni `float` ga
o'tkazadi. Busiz ham kod yig'iladi, lekin aniq yozilgani yaxshi: "bu yerda aniqlikni ataylab kamaytiryapman".

### 6.3. Yig'ib ko'ring

Rang to'q ko'kdan och ko'kka va qaytib asta o'zgarib turadi, bir to'liq aylanish ~6.3 soniya. Dastur ishlab turganda oyna markazidagi
pikselni har 0.7 soniyada o'lchadim: `(13, 100, 200)`, `(13, 69, 145)`, `(13, 40, 92)`, `(13, 51, 112)`, `(13, 88, 179)`. Qizil doim 13,
qolgan ikkitasi jadvaldagi oraliqda o'zgarib turibdi.

Oynani sichqoncha bilan sudrab turganda rang to'xtashi mumkin: Windows bu paytda hodisalar siklini ushlab turadi. Bu xato emas.

Sinab ko'ring: `glfwPollEvents()` ni vaqtincha 00-darsdagi `glfwWaitEvents()` ga qaytaring. Rang qotib qoladi: sikl hodisa kelguncha
uxlaydi va kadr chizilmaydi. Sinab ko'rganimda rang 1.5 soniya davomida bir xil qoldi. Oynaga 2 soniya davomida sichqoncha
harakati xabarlarini yuborganimda esa rang yana o'zgardi, sarlavhada 22 FPS chiqdi: har bir hodisa bitta kadr chizdirdi. 4-qadamda `glfwPollEvents` ga o'tishimizning sababi shu. Sinab bo'lgach, qaytaring.

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/main.cpp</code> (solishtirish uchun)</summary>

```cpp
#include "gl.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cmath>
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

        const double now = glfwGetTime();
        const float wave = 0.5f + 0.5f * static_cast<float>(std::sin(now));
        glClearColor(0.05f, 0.15f + 0.25f * wave, 0.35f + 0.45f * wave, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        fps_frames++;
        if (now - fps_time >= 0.5) {
            char title[128];
            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS",
                          fps_frames / (now - fps_time));
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

## 7-qadam. Oyna o'lchami va framebuffer o'lchami

Windows'da hozirgi kod to'g'ri ishlaydi, lekin Mac'ning Retina ekranida bitta nozik farq bor. Uni hozirdan hisobga olamiz, chunki
02-darsda uchburchak chizganda bu farq ko'zga tashlanadi.

### 7.1. Retina va ikki xil o'lcham

Mac'ning **Retina** ekranida piksellar shunchalik mayda va zich, oddiy hisobda chizilgan rasm juda kichik ko'rinardi. Shuning uchun
macOS oynalarni "nuqta" (point) bilan o'lchaydi va har bir nuqta odatda 2 × 2 = 4 ta haqiqiy piksel. Natijada bitta oynaning ikki
xil o'lchami bor:

- `glfwGetWindowSize`: oyna koordinatalarida, masalan 1280 × 720. Sichqoncha shu birlikda ishlaydi (07-dars).
- `glfwGetFramebufferSize`: haqiqiy piksellarda, masalan 2560 × 1440. Bu `2560 × 1440 = 3 686 400` piksel, 1280 × 720 dagidan
  4 baravar ko'p.

Windows'da ikkalasi bir xil chiqadi, Mac'da esa farq qiladi.

**`glViewport`** OpenGL'ga framebuffer'ning qaysi to'rtburchagiga chizish kerakligini aytadi: chap pastki burchak `(0, 0)` va o'lcham.
OpenGL piksellar bilan ishlaydi, shuning uchun unga **framebuffer** o'lchami beriladi. Oyna o'lchami istalgan payt o'zgarishi mumkin
(foydalanuvchi uni cho'zadi), shuning uchun o'lchamni har kadrda so'raymiz.

Bitta nozik joy: `glClear` viewport'ga qaramaydi va butun framebuffer'ni tozalaydi. Shuning uchun bu darsda noto'g'ri `glViewport`
ko'zga ko'rinmaydi. Uning xatosi uchburchak chizganda chiqadi: Retina'da rasm ekranning chap pastki choragiga siqilib qoladi.

**Ikki sonni qanday qaytarish.** C++ da funksiya bitta qiymat qaytaradi, `glfwGetFramebufferSize` esa ikki son berishi kerak. Shuning
uchun unga ikki o'zgaruvchining **manzili** beriladi (2.1-bo'limdagi uylar raqami): `&fb_width` "`fb_width` o'zgaruvchisining
manzili" degani. GLFW natijani shu manzillarga yozadi.

### 7.2. Kod

**`src/gl.hpp`.** Ro'yxatga `glViewport` ni qo'shing (`glGetString` dan keyin):

<!-- diff s6 s7 src/gl.hpp -->
```diff
 #define GL_FUNCTIONS(X)                  \
     X(PFNGLGETSTRINGPROC, glGetString)   \
+    X(PFNGLVIEWPORTPROC, glViewport)     \
     X(PFNGLCLEARCOLORPROC, glClearColor) \
```

**`src/main.cpp`.** Sikl boshida, `glfwPollEvents` dan keyin, framebuffer o'lchamini so'rab viewport'ni sozlang:

<!-- diff s6 s7 src/main.cpp -->
```diff
         glfwPollEvents();
 
+        int fb_width = 0;
+        int fb_height = 0;
+        glfwGetFramebufferSize(window, &fb_width, &fb_height);
+        glViewport(0, 0, fb_width, fb_height);
+
         const double now = glfwGetTime();
```

O'zgaruvchilarni `0` bilan boshlaymiz: GLFW biror sabab bilan yoza olmasa ham, ichida tasodifiy son qolmaydi.

Sarlavhaga ikkala o'lchamni ham chiqaramiz, shunda Mac'dagi farqni ko'z bilan ko'rasiz. `if` ichini shunday o'zgartiring:

<!-- diff s6 s7 src/main.cpp -->
```diff
         if (now - fps_time >= 0.5) {
+            int win_width = 0;
+            int win_height = 0;
+            glfwGetWindowSize(window, &win_width, &win_height);
+
             char title[128];
-            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS",
-                          fps_frames / (now - fps_time));
+            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS | oyna %dx%d | framebuffer %dx%d",
+                          fps_frames / (now - fps_time), win_width, win_height, fb_width, fb_height);
             glfwSetWindowTitle(window, title);
```

`%d` "butun son" degani. Matndagi har bir `%` ga tartib bilan bitta son to'g'ri keladi: `%.0f` ga FPS, to'rtta `%d` ga to'rtta o'lcham.

### 7.3. Yig'ib ko'ring

- Windows'da sarlavha: `Fluid Sim | 165 FPS | oyna 1280x720 | framebuffer 1280x720`.
- Oynani cho'zing: sonlar ham o'zgaradi. Men oynani 800 × 600 qildim, yarim soniyadan keyin sarlavhada
  `oyna 800x600 | framebuffer 800x600` chiqdi.
- Mac'da `oyna` va `framebuffer` **har xil** bo'lishi kutiladi, masalan `oyna 1280x720 | framebuffer 2560x1440`.

Sinab ko'rdim: `glViewport(0, 0, fb_width / 2, fb_height / 2)` qilsam ham oyna to'liq bo'yaldi. Bu 7.1-bo'limdagi gapning isboti:
`glClear` viewport'ga qaramaydi.

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/gl.hpp</code> (solishtirish uchun)</summary>

```cpp
#pragma once

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif
#include <GL/glcorearb.h>

#define GL_FUNCTIONS(X)                  \
    X(PFNGLGETSTRINGPROC, glGetString)   \
    X(PFNGLVIEWPORTPROC, glViewport)     \
    X(PFNGLCLEARCOLORPROC, glClearColor) \
    X(PFNGLCLEARPROC, glClear)

#define X(type, name) extern type name;
GL_FUNCTIONS(X)
#undef X

bool gl_load();
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/main.cpp</code> (solishtirish uchun)</summary>

```cpp
#include "gl.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cmath>
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

        const double now = glfwGetTime();
        const float wave = 0.5f + 0.5f * static_cast<float>(std::sin(now));
        glClearColor(0.05f, 0.15f + 0.25f * wave, 0.35f + 0.45f * wave, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        fps_frames++;
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

## 8-qadam. Xatolarni ushlash: `glGetError`

Oxirgi qadam xavfsizlik uchun. OpenGL xato qilganimizni o'zi aytmaydi, shuning uchun uni har kadrda o'zimiz so'raymiz.

### 8.1. Xato bayrog'i

Noto'g'ri OpenGL chaqiruvi dasturni to'xtatmaydi va hech narsa demaydi. Buyruq e'tiborsiz qoldiriladi va OpenGL ichida **xato bayrog'i**
ko'tariladi. Mashina asboblar panelidagi "dvigatelni tekshiring" chirog'ini eslang: u yonadi va kimdir qarab, sababini o'qimaguncha
yonib turadi.

Bayroqni o'qishning yagona yo'li `glGetError()`. U xato kodini qaytaradi va bayroqni tushiradi. Bir vaqtda bir nechta bayroq bo'lishi
mumkin, shuning uchun sikl bilan o'qiymiz:

```cpp
while (GLenum error = glGetError()) {
```

Bu qator bir vaqtda ikki ish qiladi: `error` o'zgaruvchisini yaratadi va unga `glGetError()` natijasini yozadi. `while` sharti esa shu
qiymatni tekshiradi: C++ da 0 "yolg'on", qolgan hamma son "rost". `GL_NO_ERROR` ning qiymati aynan 0, shuning uchun xatolar tugashi bilan
sikl to'xtaydi.

Qulayroq *debug output* (KHR_debug, xatoni o'zi xabar qiladigan mexanizm) OpenGL 4.3 da paydo bo'lgan, Mac'da u yo'q. Shuning uchun kichik
`gl_check` yordamchisini yozamiz va uni har kadr oxirida chaqiramiz.

### 8.2. Kod

**`src/gl.hpp`.** Ro'yxatning **boshiga** `glGetError` ni qo'shing (boshiga qo'shilgan qatorga ham `\` kerak, chunki undan keyin
ro'yxat davom etadi):

<!-- diff s7 s8 src/gl.hpp -->
```diff
 #define GL_FUNCTIONS(X)                  \
+    X(PFNGLGETERRORPROC, glGetError)     \
     X(PFNGLGETSTRINGPROC, glGetString)   \
```

Faylning oxiriga `gl_check` ning e'lonini qo'shing:

<!-- diff s7 s8 src/gl.hpp -->
```diff
 bool gl_load();
+void gl_check(const char* where);
```

**`src/gl.cpp`.** Faylning oxiriga, `gl_load` dan keyin, funksiyaning o'zini yozing:

<!-- append s7 s8 src/gl.cpp -->
```cpp

void gl_check(const char* where) {
    while (GLenum error = glGetError()) {
        const char* name = "noma'lum";
        switch (error) {
            case GL_INVALID_ENUM: name = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE: name = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: name = "GL_INVALID_OPERATION"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: name = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            case GL_OUT_OF_MEMORY: name = "GL_OUT_OF_MEMORY"; break;
        }
        std::fprintf(stderr, "OpenGL xatosi (%s): %s\n", where, name);
    }
}
```

- `while` 8.1-bo'limdagi sikl: bayroqlar tugaguncha o'qiydi.
- `switch` xato kodini o'qiladigan nomga aylantiradi. Kod beshtasidan biri bo'lmasa, nom `"noma'lum"` qoladi.
- `where` xato qayerda ushlanganini bildiradi (masalan `"kadr"`). Keyingi darslarda `gl_check` ni boshqa joylarda ham chaqiramiz va
  xabardan qaysi joy ekanini bilib olamiz.

**`src/main.cpp`.** `glClear` dan keyin har kadrda tekshiring:

<!-- diff s7 s8 src/main.cpp -->
```diff
         glClear(GL_COLOR_BUFFER_BIT);
+        gl_check("kadr");
 
         glfwSwapBuffers(window);
```

### 8.3. Yig'ib ko'ring

Hammasi to'g'ri bo'lsa, terminalda `OpenGL xatosi` qatorlari yo'q. Tekshiruv ishlashini ko'rish uchun `glClear` ga ataylab noto'g'ri son
bering: `glClear(GL_COLOR_BUFFER_BIT)` ni `glClear(0xFFFFFFFF)` ga o'zgartiring. Sinab ko'rdim: terminalga har kadrda shu qator chiqdi,
oyna esa qora qoldi, chunki tozalash umuman bajarilmadi:

```
OpenGL xatosi (kadr): GL_INVALID_VALUE
```

Xuddi shu xatoni `gl_check("kadr")` qatorisiz ham sinadim: oyna qora, terminal jim. Xatoning sababini topish uchun hech qanday
ishora yo'q. `gl_check` aynan shu bo'shliqni to'ldiradi. Sinab bo'lgach, `GL_COLOR_BUFFER_BIT` ni qaytaring.

Bu qadamdan keyingi fayllar darsning yakuniy kodi:

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/gl.hpp</code> (solishtirish uchun)</summary>

```cpp
#pragma once

#if defined(_WIN32) && !defined(APIENTRY)
#define APIENTRY __stdcall
#endif
#include <GL/glcorearb.h>

#define GL_FUNCTIONS(X)                  \
    X(PFNGLGETERRORPROC, glGetError)     \
    X(PFNGLGETSTRINGPROC, glGetString)   \
    X(PFNGLVIEWPORTPROC, glViewport)     \
    X(PFNGLCLEARCOLORPROC, glClearColor) \
    X(PFNGLCLEARPROC, glClear)

#define X(type, name) extern type name;
GL_FUNCTIONS(X)
#undef X

bool gl_load();
void gl_check(const char* where);
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/gl.cpp</code> (solishtirish uchun)</summary>

```cpp
#include "gl.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cstdio>

#define X(type, name) type name = nullptr;
GL_FUNCTIONS(X)
#undef X

bool gl_load() {
#define X(type, name)                                                     \
    name = reinterpret_cast<type>(glfwGetProcAddress(#name));             \
    if (!name) {                                                          \
        std::fprintf(stderr, "OpenGL funksiyasi topilmadi: %s\n", #name); \
        return false;                                                     \
    }
    GL_FUNCTIONS(X)
#undef X
    return true;
}

void gl_check(const char* where) {
    while (GLenum error = glGetError()) {
        const char* name = "noma'lum";
        switch (error) {
            case GL_INVALID_ENUM: name = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE: name = "GL_INVALID_VALUE"; break;
            case GL_INVALID_OPERATION: name = "GL_INVALID_OPERATION"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: name = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
            case GL_OUT_OF_MEMORY: name = "GL_OUT_OF_MEMORY"; break;
        }
        std::fprintf(stderr, "OpenGL xatosi (%s): %s\n", where, name);
    }
}
```

</details>

<details>
<summary>Shu qadamdan keyingi to'liq <code>src/main.cpp</code> (solishtirish uchun)</summary>

```cpp
#include "gl.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <cmath>
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

        const double now = glfwGetTime();
        const float wave = 0.5f + 0.5f * static_cast<float>(std::sin(now));
        glClearColor(0.05f, 0.15f + 0.25f * wave, 0.35f + 0.45f * wave, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        gl_check("kadr");

        glfwSwapBuffers(window);

        fps_frames++;
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

## 9. Yakuniy tekshiruv

**Windows.** Bu natijani o'zim ko'rdim. Terminalda:

```
GLFW:   3.5.1 Win32 WGL Null EGL OSMesa MinGW-w64 DLL
OpenGL: 4.1.0 NVIDIA 610.47
GLSL:   4.10 NVIDIA via Cg compiler
GPU:    NVIDIA GeForce RTX 3060/PCIe/SSE2
```

- Oyna to'q ko'kdan och ko'kka va qaytib asta o'zgarib turadi.
- Sarlavhada: `Fluid Sim | 165 FPS | oyna 1280x720 | framebuffer 1280x720`.
- Terminalda `OpenGL xatosi` qatorlari yo'q.
- X oynani yopadi, `echo $LASTEXITCODE` `0` beradi.

**macOS.** Buni o'zim ishga tushira olmadim, siz esa ishlaganini aytgansiz. Kutilayotgan natija:

- `OpenGL:` qatori `4.1 Metal - ...` ko'rinishida, `GLSL:` qatori `4.10`, `GPU:` qatorida Mac'ingizning chipi.
- Rang xuddi shunday o'zgarib turadi, sarlavhadagi FPS monitoringiz chastotasiga teng (60 yoki 120).
- Sarlavhada `oyna` va `framebuffer` **har xil** (Retina), masalan `oyna 1280x720 | framebuffer 2560x1440`.

**Nima buzilsa nima bo'ladi: hammasi bir joyda.** Har bir qator RTX 3060 da ishga tushirib ko'rilgan:

| O'zgarish | Natija | Xabar bormi | Qadam |
|---|---|---|---|
| `GLFW_NO_API` qolib ketgan | kontekst yo'q | `GLFW xatosi 65546` | 1 |
| `GLFW_OPENGL_FORWARD_COMPAT` yo'q | Windows'da o'zgarish yo'q, Mac'da kontekst berilmasligi kutiladi | Windows'da yo'q | 1 |
| `gl_load` kontekstdan oldin | dastur `1` kodi bilan chiqadi | ikki qator | 2 |
| `gl_load` umuman yo'q | oyna oq, dastur yiqiladi (`0xC0000005`) | yo'q | 2 |
| `target_include_directories` yo'q | Windows'da yig'iladi, Mac'da xato kutiladi | Windows'da yo'q | 2 |
| `APIENTRY` bloki yo'q, `float near` | kompilyatsiya xatosi | `expected unqualified-id` | 2 |
| ro'yxatga `glDispatchCompute` (4.3) | Windows'da topiladi, Mac'da topilmasligi kerak | Windows'da yo'q | 3 |
| `glfwSwapBuffers` yo'q | oyna oq | yo'q | 4, 5 |
| `glfwWaitEvents` qaytib qolgan | rang qotadi | yo'q | 6 |
| `glClear(0xFFFFFFFF)` | oyna qora | har kadrda `GL_INVALID_VALUE` | 8 |

Jadvalda "Windows'da yo'q" degan qatorlarga alohida e'tibor bering: Mac'ni buzadigan xatoni Windows sizga ko'rsatmaydi.

## 10. Git va Mac

Windows'da:

```
git add .
git commit -m "Lesson 01: OpenGL 4.1 context and function loading"
git push
```

Mac'da:

```
git pull
cmake --build build
./build/fluid_sim
```

## 11. Lug'at

| So'z | Ma'nosi |
|---|---|
| GPU | minglab oddiy ishchidan iborat hisoblovchi, rasm chizadi |
| drayver | buyruqlarimizni GPU tiliga tarjima qiladigan dastur |
| spetsifikatsiya | "nima bo'lishi kerak" degan qoidalar hujjati |
| kontekst | OpenGL'ning sozlamalari va resurslari saqlanadigan "ustaxona" |
| oqim (thread) | dastur ichidagi bitta bajarish yo'li |
| core profil | OpenGL'ning eski uslubsiz qismi |
| hint | oyna yaratishdan oldin GLFW'ga beriladigan talab |
| manzil | xotiradagi joyning raqami |
| funksiya ko'rsatkichi | funksiya manzilini saqlaydigan o'zgaruvchi |
| funksiyalarni yuklash | drayverdan funksiya manzillarini so'rab olish |
| e'lon / ta'rif | "shunday narsa bor" / narsaning o'zi |
| makro | preprocessor almashtiradigan nom |
| X-macro | bitta ro'yxatni turli `X` bilan bir necha marta ochish usuli |
| refactoring | natijani o'zgartirmasdan kodni qayta tuzish |
| diff | fayldagi o'zgarishni `+` (qo'shish) va `-` (o'chirish) bilan ko'rsatish |
| piksel | ekrandagi eng kichik nuqta |
| RGB | rang uchta son bilan: qizil, yashil, ko'k (0 dan 1 gacha) |
| alpha | shaffoflik: 1 to'liq ko'rinadi, 0 ko'rinmaydi |
| framebuffer | xotiradagi rasm, har bir piksel uchun rang |
| holat mashinasi | sozlama o'zgartirilguncha saqlanib turadigan tizim |
| front / back buffer | ekrandagi rasm / chizilayotgan rasm |
| kadr (frame), FPS | bitta chizilgan rasm, soniyasiga nechta kadr |
| Hz | soniyasiga necha marta |
| tearing | ekranda ikki kadr aralashib, "yirtilib" ko'rinishi |
| vsync | buffer almashtirishni monitor yangilanishiga moslash |
| sinus | −1 va 1 orasida silliq borib-keladigan son |
| Retina | piksellari juda zich ekran, bitta nuqta = 2 × 2 piksel |
| viewport | framebuffer'ning chizish tushadigan to'rtburchagi |
| xato bayrog'i | `glGetError` o'qiguncha saqlanadigan xato kodi |

## Keyingi dars

02-darsda birinchi uchburchakni chizamiz: GLSL 410 shader'lar, VAO va VBO, va GPU'ga ma'lumot yuborish.

Shu dars tugagach yozing. Xohlasangiz, kodingizni ko'rib chiqaman.
