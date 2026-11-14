# 05. Zichlik: SPH g'oyasi va smoothing kernel

## Bu qadamda nima qo'shamiz va nega aynan hozir

04-darsda zarrachalar bir-biridan bemalol o'tib ketdi va oxirida hammasi polda bitta chiziqqa yig'ildi. Haqiqiy suyuqlik bunday qilmaydi:
suvni siqmoqchi bo'lsangiz, u qarshilik ko'rsatadi. Bu qarshilik **bosim** deyiladi va u 06-darsda keladi.

Bosimni hisoblash uchun avval har bir joy qanchalik "tiqilinch" ekanini bilishimiz kerak. Bu **zichlik**. Tiqilinch joyda bosim katta bo'lib,
zarrachalarni bo'shroq joyga itaradi. Shuning uchun bu darsda faqat zichlikni hisoblaymiz va uni rang bilan ko'rsatamiz. Harakat o'zgarmaydi,
zarrachalar hali ham bir-biridan o'tib ketadi, lekin endi qayerda tiqilinch, qayerda siyrak ekanini ko'rasiz.

Bu dars SPH'ning yuragi. **SPH** (Smoothed Particle Hydrodynamics, "silliqlangan zarrachalar gidrodinamikasi") suyuqlikni minglab kichik
zarrachalar bilan ifodalaydi. Har bir zarracha suyuqlikning kichik bir bo'lagi, har bir joydagi qiymat esa (zichlik, bosim) atrofdagi
zarrachalardan "silliqlab" hisoblanadi. Bu "silliqlash" nima ekanini shu darsda ko'ramiz.

Avvalgidek, oldindan fizika yoki matematika bilish shart emas. Har bir tushuncha noldan tushuntiriladi, raqamlarning hammasi dasturda
hisoblab tekshirilgan. Dars oxirida qisqa lug'at bor.

Loyihada shular o'zgaradi (`CMakeLists.txt`, `gl.hpp` va `particle2d.frag` o'zgarmaydi, yangi GL funksiyasi kerak emas):

```
opengl_fluid_sim_scratch/
├── shaders/
│   └── particle2d.vert     (o'zgaradi)
└── src/
    ├── sim2d.hpp           (o'zgaradi)
    ├── sim2d.cpp           (o'zgaradi)
    └── main.cpp            (o'zgaradi)
```

Yana bitta o'zgarish bor: blokni ikki baravar kichraytiramiz (1148 zarracha). Nega bunday qilishimiz 6-bo'limda o'lchovlar bilan tushuntiriladi.

## 1. Zichlik nima

Avtobusni tasavvur qiling. Ertalab ichida 80 kishi, kechqurun 10 kishi bor. Avtobus o'sha-o'sha, lekin ertalab tiqilinch. "Tiqilinch" ni son bilan
aytish uchun odamlar sonini joyga bo'lamiz: "bir kvadrat metrda necha kishi". Mana shu **zichlik**.

Bizda har bir zarrachaning massasi 1 deb olinadi, shuning uchun zichlik oddiygina "bir kvadrat birlikda nechta zarracha". **Kvadrat birlik** bu tomoni
1 birlik bo'lgan kvadratning yuzi. To'g'ri to'rtburchakning yuzi eni × bo'yi.

04-darsda spawner'ga `density = 159` bergan edik. Bu "har bir kvadrat birlikka 159 ta zarracha" degani. Tekshirib ko'ramiz. Bu darsdagi blok
3.21 × 2.195, uning yuzi `3.21 × 2.195 = 7.05` kvadrat birlik. Ichida 1148 zarracha bor, demak o'rtacha zichlik `1148 / 7.05 = 163`. 159 ga yaqin.

Lekin bizga butun blokning o'rtachasi emas, **har bir zarracha turgan joydagi** zichlik kerak. Suyuqlik bir joyda siqilib, boshqa joyda siyraklashishi
mumkin, bosim esa aynan shu farqdan paydo bo'ladi.

## 2. Birinchi urinish: doira ichidagilarni sanash

Eng oddiy fikr: har bir zarracha atrofida radiusi `r` bo'lgan doira chizamiz, ichidagi zarrachalarni sanaymiz va doiraning yuziga bo'lamiz.

**Doiraning yuzi.** Har qanday doiraning yuzi radiusining kvadratidan (radiusni o'ziga ko'paytirishdan) taxminan 3.14 baravar katta. Bu 3.14159...
soni **pi** deyiladi. Bizning radius `r = 0.35`, shuning uchun doira yuzi `3.1416 × 0.35 × 0.35 = 0.385`. Agar doira ichida 61 ta zarracha bo'lsa,
zichlik `61 / 0.385 = 158`.

Bu usulda jiddiy kamchilik bor. Qo'shni zarracha doira chetidan o'tganda nima bo'lishini ko'ramiz:

| qo'shni qayerda | sanash usuli qo'shadigan zichlik |
|---|---|
| 0.330 uzoqlikda (ichkarida) | 2.6 |
| 0.340 | 2.6 |
| 0.349 (chetga juda yaqin) | 2.6 |
| 0.351 (chetdan tashqari) | 0 |

Zarracha atigi 0.002 birlik siljidi, zichlik esa birdaniga 2.6 ga sakradi (`1 / 0.385 = 2.6`). 06-darsda zichlikdan kuch hisoblaymiz, ya'ni
zichlikdagi har bir sakrash kuchdagi sakrash bo'ladi va zarrachalar titray boshlaydi. Bizga **silliq** o'zgaradigan zichlik kerak.

## 3. Smoothing kernel: yaqinroq qo'shni ko'proq hisoblanadi

Gulxanni eslang. Yonida turganda issiq, biroz uzoqlashsangiz iliq, ma'lum masofadan keyin issiqlik umuman sezilmaydi. Issiqlik birdaniga uzilmaydi,
asta-sekin kamayadi.

Qo'shnilarni ham shunday hisoblaymiz. Har bir qo'shniga masofasiga qarab **vazn** (weight) beramiz: yaqindagisi katta vazn oladi, uzoqroqdagisi
kichik, radius chetida esa vazn 0 ga tushadi. Masofadan vazn beradigan bu qoida **smoothing kernel** deyiladi ("silliqlovchi yadro",
hamma joyda inglizcha nomi ishlatiladi).

Biz asl Fluid-Sim'dagi kernel'ni olamiz:

```
vazn = (r - d) × (r - d)          agar d < r bo'lsa
vazn = 0                          agar d ≥ r bo'lsa
```

Bu yerda `d` qo'shnigacha masofa, `r` radius (0.35). `r - d` "chetgacha qancha qoldi" degani, uni o'ziga ko'paytiramiz. Sonlar bilan ko'ramiz:

| masofa `d` | `r - d` | vazn `(r - d) × (r - d)` |
|---|---|---|
| 0 | 0.35 | 0.1225 |
| 0.05 | 0.30 | 0.0900 |
| 0.10 | 0.25 | 0.0625 |
| 0.15 | 0.20 | 0.0400 |
| 0.20 | 0.15 | 0.0225 |
| 0.25 | 0.10 | 0.0100 |
| 0.30 | 0.05 | 0.0025 |
| 0.35 | 0 | 0 |

Grafigi tepalikka o'xshaydi: markazda baland, chetga qarab pasayadi.

```
vazn
  ▲
  █
  ██
  ███
  █████
  ███████
  ██████████
  █████████████████
  ──────────────────► masofa
  0                0.35
```

**Nega aynan o'ziga ko'paytiramiz?** `r - d` ning o'zi ham chetda 0 bo'lardi, lekin to'g'ri chiziq bo'lib tushardi. Kichik sonni o'ziga ko'paytirsak,
u yanada kichrayadi: `0.05 × 0.05 = 0.0025`. Natijada chetga yaqinlashganda vazn juda yumshoq nolga yaqinlashadi va doira chetidan o'tish deyarli sezilmaydi.
2-bo'limdagi jadvalni kernel bilan qaytadan hisoblasak (vazn keyingi bo'limdagi koeffitsientga ko'paytirilgan):

| qo'shni qayerda | sanash usuli | kernel |
|---|---|---|
| 0.330 | 2.6 | 0.051 |
| 0.340 | 2.6 | 0.013 |
| 0.349 | 2.6 | 0.0001 |
| 0.351 | 0 | 0 |

Sakrash yo'qoldi: qo'shni chiqib ketayotganda uning hissasi oldindan asta-sekin nolga tushib boradi.

## 4. Normalizatsiya: tepalikning hajmi 1 bo'lishi kerak

Endi zichlik = hamma qo'shnilar vaznining yig'indisi. Lekin vaznlar juda kichik sonlar (eng kattasi 0.1225). Blok ichidagi zarracha uchun yig'indini hisobladim,
**1.217** chiqdi. Bizga esa 159 atrofida kerak. Vaznlarni qandaydir songa ko'paytirishimiz kerak, lekin qaysi songa?

**Qum uyumi.** Har bir zarrachani 1 kg qum deb tasavvur qiling. Qumni bir nuqtaga to'kish o'rniga, zarracha atrofidagi doira ustiga kernel shaklida
uyum qilib sepamiz: markazda baland, chetida nol. Biror joydagi zichlik shu joyda ustma-ust tushgan hamma uyumlar balandligining yig'indisi.

Har bir uyumda aniq 1 kg qum bo'lsa, ustma-ust tushgan uyumlar aynan shu joyga to'g'ri kelgan massani beradi. Uyumda 2 kg bo'lsa, hamma zichliklar ikki
baravar katta chiqadi. 0.008 kg bo'lsa, 0.008 baravar. Demak tepalikning **hajmi** (ichidagi qum miqdori) aniq 1 bo'lishi kerak. Bunga **normalizatsiya** deyiladi.

`(r - d) × (r - d)` tepaligining hajmini topamiz, keyin vaznni shu hajmga bo'lamiz. Shunda hajm 1 bo'ladi.

**Hajm.** Quti ichiga qancha narsa sig'ishi uning hajmi: eni × bo'yi × balandligi. 2 × 3 o'lchamli, 0.5 balandlikdagi qutining hajmi `2 × 3 × 0.5 = 3`.

Tepalik quti emas, uning balandligi har joyda har xil. Shuning uchun uni **ingichka halqalarga** bo'lamiz, xuddi daraxt kesimidagi yillik halqalar kabi.
Bitta ingichka halqani kesib, to'g'rilasak, u deyarli yupqa quti bo'ladi:

```
   yuqoridan ko'rinishi                 bitta halqa, to'g'rilangan

      .-''''''-.
    /  .-''''-.  \              ┌───────────────────────────┐ ← balandlik
   |  |   ●    |  |             └───────────────────────────┘
    \  '-....-'  /               ◄────── halqa uzunligi ─────►
      '-......-'
```

- **Uzunligi** halqaning aylanasi. Har qanday aylana diametridan pi baravar uzun, ya'ni `2 × pi × radius`. Radiusi 1 bo'lgan g'ildirak bir aylanganda
  `2 × 3.1416 × 1 = 6.28` birlik yuradi.
- **Eni** halqaning qalinligi.
- **Balandligi** tepalikning shu joydagi balandligi, ya'ni vazn.

Hisob oson bo'lishi uchun avval `r = 1` olamiz va tepalikni qalinligi 0.2 bo'lgan 5 ta halqaga bo'lamiz. Har bir halqa uchun uning o'rtasidagi masofani olamiz:

| halqa | o'rtasi `d` | uzunlik `2 × pi × d` | balandlik `(1 - d) × (1 - d)` | hajm = uzunlik × 0.2 × balandlik |
|---|---|---|---|---|
| 1 | 0.1 | 0.6283 | 0.81 | 0.1018 |
| 2 | 0.3 | 1.8850 | 0.49 | 0.1847 |
| 3 | 0.5 | 3.1416 | 0.25 | 0.1571 |
| 4 | 0.7 | 4.3982 | 0.09 | 0.0792 |
| 5 | 0.9 | 5.6549 | 0.01 | 0.0113 |
| | | | **jami** | **0.5341** |

Halqa ichida balandlik biroz o'zgaradi, biz esa o'rtasidagini oldik, shuning uchun natija taxminiy. Halqalarni ingichkalashtirsak, aniqlashadi:

| halqalar soni | hajm |
|---|---|
| 1 | 0.7854 |
| 2 | 0.5890 |
| 5 | 0.5341 |
| 10 | 0.5262 |
| 100 | 0.52362 |
| 1000 | 0.52360 |

Son 0.52360 da to'xtadi. Bu `pi / 6 = 3.1416 / 6 = 0.5236`. Matematikada "juda ko'p ingichka bo'laklarni qo'shib, aniq hajmni topish" usuli **integral** deyiladi.
Hozircha nomini bilish yetarli.

**Boshqa radiuslar.** Radius 2 baravar katta bo'lsa nima bo'ladi? Tepalik eniga ham, bo'yiga ham 2 baravar kengayadi, ya'ni egallagan yuzi 4 baravar katta.
Balandligi `(r - d) × (r - d)` ham 4 baravar oshadi (`2 × 2`). Jami hajm `4 × 4 = 16` baravar, bu `2 × 2 × 2 × 2`. Demak hajm radiusni **to'rt marta o'ziga
ko'paytirganga** proporsional:

```
hajm = 0.5236 × r × r × r × r
```

Tekshirdim: `r = 2` da hajm 8.3776 chiqdi, bu aynan `16 × 0.5236`. `r = 0.35` da 0.007857, bu `0.35 × 0.35 × 0.35 × 0.35 × 0.5236`.

**Koeffitsient.** Hajmni 1 qilish uchun vaznni hajmga bo'lamiz, ya'ni `1 / hajm` ga ko'paytiramiz:

```
koeffitsient = 1 / (pi / 6 × r × r × r × r) = 6 / (pi × r × r × r × r)
```

`r = 0.35` uchun: `0.35 × 0.35 × 0.35 × 0.35 = 0.0150`, `pi × 0.0150 = 0.0471`, `6 / 0.0471 = 127.3`. Kodda bu `scale`. Endi kernel jadvali shunday:

| masofa `d` | 0 | 0.05 | 0.10 | 0.15 | 0.20 | 0.25 | 0.30 | 0.35 |
|---|---|---|---|---|---|---|---|---|
| vazn × 127.3 | 15.59 | 11.45 | 7.95 | 5.09 | 2.86 | 1.27 | 0.32 | 0 |

Blok ichidagi zarracha uchun yig'indi `1.217 × 127.3 = 155`. Spawner'dagi 159 ga juda yaqin. Kernel to'g'ri ishlayotganini mana shundan bilamiz.

**Nega 127.3 ni qo'lda yozmaymiz?** Keyingi sahnalarda radius boshqacha bo'ladi (0.2, 0.175). Formula har qanday radiusga o'zi moslashadi. Blok ichidagi
zichlikni uch xil radius bilan o'lchadim:

| radius | koeffitsientsiz | koeffitsient bilan | blok ichida eng kichigi va kattasi |
|---|---|---|---|
| 0.2 | 0.136 | 162 | 147 ... 178 |
| 0.35 | 1.217 | 155 | 151 ... 160 |
| 0.5 | 5.032 | 154 | 152 ... 156 |

Koeffitsientsiz natija radiusga qarab 40 baravar o'zgaradi. Koeffitsient bilan esa hamma joyda 159 atrofida. Kichik radiusda farq kattaroq (147 dan 178 gacha), chunki
doira ichiga atigi 20 ta qo'shni tushadi va har birining tasodifiy joyi ko'proq ta'sir qiladi. `r = 0.35` da qo'shnilar 61 ta, `r = 0.5` da 125 ta.

Natija 159 emas, 155 chiqishining sababi oddiy: zarrachalar alohida nuqtalar, tekis taralgan qum emas. Bu taxminiy hisob, lekin bizga yetarlicha aniq.

## 5. Zichlikni hisoblash

Har bir zarracha uchun:

```
zichlik = hamma zarrachalar bo'yicha vazn(masofa) yig'indisi
```

**O'zi ham hisobga kiradi.** Zarrachaning o'z qum uyumi ham o'z joyida turibdi, masofa 0, vazn 15.59. Usiz blok ichidagi zichlik 155 emas, 139 chiqadi.

**Ikki nuqta orasidagi masofa.** Zarracha `(1, 1)` da, qo'shni `(4, 5)` da. Qo'shniga borish uchun o'ngga `dx = 4 - 1 = 3` va yuqoriga `dy = 5 - 1 = 4`
yurish kerak. To'g'ri chiziq bo'ylab masofa qancha?

```
        (4, 5)
          ●
         /│
        / │
     ? /  │ dy = 4
      /   │
     /    │
    ●─────┘
 (1, 1)  dx = 3
```

`dx`, `dy` va to'g'ri chiziq to'g'ri burchakli uchburchak hosil qiladi. Uning uchun qadimdan ma'lum qoida bor (**Pifagor teoremasi**): eng uzun tomonning kvadrati
qolgan ikki tomon kvadratlarining yig'indisiga teng.

```
masofa × masofa = dx × dx + dy × dy = 3 × 3 + 4 × 4 = 9 + 16 = 25
masofa = sqrt(25) = 5
```

`sqrt` (kvadrat ildiz) 04-darsdan tanish: qaysi son o'ziga ko'paytirilsa, 25 chiqadi. Javob 5.

**Kvadratlarni solishtirish.** Kvadrat ildiz olish ko'paytirishdan ancha sekin. Zarrachalarning ko'pchiligi esa radiusdan uzoqda. 1148 zarrachadan atigi 60 tachasi
doiraga tushadi, ya'ni 95% juftlik uchun aniq masofa umuman kerak emas. Musbat sonlarda "masofa radiusdan kichikmi?" degan savol bilan "masofa × masofa
radius × radiusdan kichikmi?" degan savol bir xil javob beradi. Shuning uchun avval kvadratlarni solishtiramiz, ildizni faqat doira ichidagilar uchun olamiz.
O'lchov:

| zarrachalar | avval kvadratni solishtirish | har juftlik uchun ildiz | natija |
|---|---|---|---|
| 1148 | 1.07 ms | 1.41 ms | bir xil |
| 4536 | 13.4 ms | 19.7 ms | bir xil |

Kodda shu:

```cpp
for (std::size_t j = 0; j < count; j++) {
    const float dx = sim.positions[j].x - p.x;
    const float dy = sim.positions[j].y - p.y;
    const float sqr_distance = dx * dx + dy * dy;
    if (sqr_distance >= sqr_radius) {
        continue;
    }
    density += density_kernel(std::sqrt(sqr_distance), radius, scale);
}
```

`continue` "bu qo'shnini tashla, keyingisiga o't" degani. `j` sikli `i` ning o'zini ham aylanib chiqadi, shuning uchun o'z hissasi avtomatik qo'shiladi.

**Qachon hisoblanadi.** `step_simulation` endi uch qismdan iborat: gravitatsiya, zichlik, o'rinni yangilash. 06-darsda bosim zichlik bilan o'rin orasiga kiradi.

## 6. Nega blokni kichraytirdik: O(n²)

Har bir zarracha qolgan **hamma** zarrachalarni tekshiradi. 1148 zarracha bo'lsa, `1148 × 1148 = 1.3 million` juftlik. 4536 zarracha bo'lsa, `4536 × 4536 = 20.6 million`.
Zarrachalar 4 baravar ko'p bo'lsa, juftliklar `4 × 4 = 16` baravar ko'p. Bunday o'sish **O(n²)** deb yoziladi ("en kvadrat tartibida" deb o'qiladi): ish zarrachalar
sonining kvadratiga qarab o'sadi. Bitta zichlik hisobi uchun ketgan vaqt:

| zarrachalar | juftliklar | bitta hisob | kadrda (3 qadam) |
|---|---|---|---|
| 988 | 1.0 million | 0.53 ms | 1.6 ms |
| 1148 | 1.3 million | 0.74 ms | 2.2 ms |
| 4536 | 20.6 million | 11.0 ms | 33 ms |
| 17 064 | 291 million | 150 ms | 450 ms |

04-darsdagi to'liq blok (4536 zarracha) bilan dastur 23–28 FPS ko'rsatdi, kompyuter boshqa ishlar bilan band bo'lganda esa 13–19. 04-darsdagi cheklov tufayli
bitta kadrda simulyatsiya `1/60` soniyadan ko'p yurmaydi, shuning uchun hamma narsa sekinlashdi: 9.6 soniyada atigi 3.7 soniyalik harakat bo'ldi, ya'ni 2.6 baravar sekin.
06-darsda bosim uchun yana bitta shunday sikl qo'shiladi va ahvol yanada yomonlashadi.

Shuning uchun blokning enini ham, bo'yini ham ikki baravar kichraytiramiz: 3.21 × 2.195. Zichlik o'sha-o'sha 159, zarrachalar orasidagi masofa ham o'sha, faqat
suyuqlik kamroq: `41 × 28 = 1148` zarracha. Bu bilan dastur yana 165 FPS da ishlaydi. 08-darsda har bir zarracha faqat yaqin qo'shnilarini tekshiradigan
bo'ladi va to'liq blokni qaytaramiz.

## 7. Zichlikni rang bilan ko'rsatish

Har bir zarrachaning zichligini GPU'ga yuboramiz, rangni esa vertex shader hisoblaydi. 04-darsdagi `color_vbo` o'rnini `density_vbo` egallaydi.

Nega rangni C++ da hisoblamaymiz? Zichlik bitta `float` (4 bayt), rang esa uchta (12 bayt), ya'ni yuboriladigan ma'lumot 3 baravar kam. Bundan tashqari rang fizika emas,
faqat ko'rinish, unga shader'da joy bor.

**Rang shkalasi.** Zichlikni spawner'dagi 159 ga bo'lamiz va `t` sonini olamiz:

- `t = 0` (zichlik 0): ko'k;
- `t = 1` (zichlik 159): oq;
- `t = 2` va undan katta (318 dan yuqori): qizil.

Oraliqdagi ranglarni **`mix`** funksiyasi topadi: `mix(a, b, t) = a + (b - a) × t`. `t = 0` da `a`, `t = 1` da `b`, `t = 0.5` da o'rtasi. Masalan ko'k
`(0.15, 0.35, 0.90)` bilan oq `(0.95, 0.95, 0.95)` ning o'rtasi: `(0.55, 0.65, 0.925)`, och ko'k. Rang uchta son (qizil, yashil, ko'k), har biri 0 dan 1 gacha,
`mix` har birini alohida aralashtiradi.

```glsl
vec3 density_color(float density) {
    float t = density / u_reference_density;
    if (t < 1.0) {
        return mix(LOW_COLOR, MID_COLOR, t);
    }
    return mix(MID_COLOR, HIGH_COLOR, min(t - 1.0, 1.0));
}
```

`min(t - 1.0, 1.0)` ikki sondan kichigini oladi, shuning uchun 318 dan yuqori hamma zichlik bir xil qizil bo'ladi. Ekranda nimalar ko'rinadi:

| qayerda | zichlik | `t` | rang |
|---|---|---|---|
| blok burchagi | 65 | 0.41 | ko'k |
| blok cheti | 100 | 0.63 | och ko'k |
| blok ichi | 155 | 0.97 | deyarli oq |
| qatorlar ustma-ust tushganda | 300+ | 2 atrofida | qizil |
| polda yotgan chiziq | 1255 | 7.9 | qizil |

Blok chetida zichlik kam, chunki zarracha atrofidagi doiraning yarmi bo'sh: u yerda qo'shni yo'q.

**Attribute.** Zichlik bitta son, shuning uchun `glVertexAttribPointer` ning ikkinchi argumenti 1, qadam esa `sizeof(float)`:

```cpp
glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);
```

Shader'da bu `layout(location = 1) in float in_density;`. Fragment shader o'zgarmaydi: u avvalgidek `v_color` ni oladi.

**Uniform.** `u_reference_density` ga spawner zichligi (159) beriladi. Shader butun shkalani shu songa qarab quradi.

## 8. Boshlang'ich holat va reset

04-darsda R tugmasi `sim = initial` qilardi. Endi `initial` ichida zichliklar ham bor, ular esa hali hisoblanmagan. Pauzada R bossangiz, birinchi qadam
bo'lmaguncha hamma zarracha ko'k bo'lib turardi. Shuning uchun `calculate_densities` ni tashqariga ochamiz (header'ga yozamiz) va boshlang'ich holat uchun
bir marta chaqiramiz:

```cpp
Simulation initial = spawn_particles(spawn);
calculate_densities(initial, settings);
Simulation sim = initial;
```

`initial` endi `const` emas, chunki uni yaratgandan keyin o'zgartiramiz.

**`densities.resize`.** `sim.densities[i] = density` vektorda allaqachon bor joyga yozadi, yangi joy ochmaydi. Shuning uchun spawner oxirida vektorga zarrachalar
soniga teng joy ajratamiz: `resize` yangi joylarni 0 bilan to'ldiradi. Usiz dastur ishga tushishi bilan yiqiladi (o'zim sinadim).

**Tekshiruv qatori.** Dastur boshida eng katta zichlikni chiqaramiz. `std::max_element` vektordagi eng katta elementni topadi va unga ko'rsatkichga o'xshash narsa
(iterator) qaytaradi, `*` esa shu elementning qiymatini oladi. Terminalda 159 atrofidagi son chiqsa, kernel va koeffitsient to'g'ri.

## 9. Kodni yozamiz

### 9.1. `src/sim2d.hpp` (to'liq almashtiring)

```cpp
#pragma once

#include <vector>

struct Vec2 {
    float x, y;
};

inline Vec2 operator*(Vec2 v, float s) {
    return {v.x * s, v.y * s};
}

inline Vec2& operator+=(Vec2& a, Vec2 b) {
    a.x += b.x;
    a.y += b.y;
    return a;
}

struct SimSettings {
    float gravity = -12.0f;
    float collision_damping = 0.95f;
    float smoothing_radius = 0.35f;
    Vec2 bounds_size = {17.1f, 9.3f};
    float max_frame_time = 1.0f / 60.0f;
    int iterations_per_frame = 3;
};

struct SpawnSettings {
    Vec2 centre = {0.0f, 0.66f};
    Vec2 size = {3.21f, 2.195f};
    float density = 159.0f;
    float jitter = 0.03f;
    Vec2 initial_velocity = {0.0f, 0.0f};
};

struct Simulation {
    std::vector<Vec2> positions;
    std::vector<Vec2> velocities;
    std::vector<float> densities;
};

Simulation spawn_particles(const SpawnSettings& spawn);
void calculate_densities(Simulation& sim, const SimSettings& settings);
void step_simulation(Simulation& sim, const SimSettings& settings, float dt);
```

04-darsdan farqlari: `smoothing_radius` (doira radiusi), blokning yangi `size`, `Simulation` ichida `densities` va yangi `calculate_densities` e'loni.

### 9.2. `src/sim2d.cpp` (to'liq almashtiring)

Sizda `resolve_collision` va `update_position` nomlari bor edi. Fayl to'liq almashadi, shuning uchun nomlar darsdagidek bo'ladi.

```cpp
#include "sim2d.hpp"

#include <cmath>
#include <cstddef>
#include <random>

static const float pi = 3.14159265f;

Simulation spawn_particles(const SpawnSettings& spawn) {
    const float per_unit = std::sqrt(spawn.density);
    const int nx = static_cast<int>(std::ceil(spawn.size.x * per_unit));
    const int ny = static_cast<int>(std::ceil(spawn.size.y * per_unit));

    Simulation sim;
    sim.positions.reserve(nx * ny);
    sim.velocities.reserve(nx * ny);

    std::mt19937 rng(42);
    std::uniform_real_distribution<float> random01(0.0f, 1.0f);

    for (int j = 0; j < ny; j++) {
        for (int i = 0; i < nx; i++) {
            const float tx = i / (nx - 1.0f);
            const float ty = j / (ny - 1.0f);

            const float angle = random01(rng) * 2.0f * pi;
            const float jitter = spawn.jitter * (random01(rng) - 0.5f);

            Vec2 position;
            position.x = spawn.centre.x + (tx - 0.5f) * spawn.size.x + std::cos(angle) * jitter;
            position.y = spawn.centre.y + (ty - 0.5f) * spawn.size.y + std::sin(angle) * jitter;

            sim.positions.push_back(position);
            sim.velocities.push_back(spawn.initial_velocity);
        }
    }

    sim.densities.resize(sim.positions.size());
    return sim;
}

static float density_kernel(float distance, float radius, float scale) {
    if (distance >= radius) {
        return 0.0f;
    }
    const float v = radius - distance;
    return v * v * scale;
}

void calculate_densities(Simulation& sim, const SimSettings& settings) {
    const float radius = settings.smoothing_radius;
    const float sqr_radius = radius * radius;
    const float scale = 6.0f / (pi * radius * radius * radius * radius);
    const std::size_t count = sim.positions.size();

    for (std::size_t i = 0; i < count; i++) {
        const Vec2 p = sim.positions[i];
        float density = 0.0f;

        for (std::size_t j = 0; j < count; j++) {
            const float dx = sim.positions[j].x - p.x;
            const float dy = sim.positions[j].y - p.y;
            const float sqr_distance = dx * dx + dy * dy;
            if (sqr_distance >= sqr_radius) {
                continue;
            }
            density += density_kernel(std::sqrt(sqr_distance), radius, scale);
        }

        sim.densities[i] = density;
    }
}

static void apply_gravity(Simulation& sim, const SimSettings& settings, float dt) {
    const Vec2 acceleration = {0.0f, settings.gravity};

    for (Vec2& velocity : sim.velocities) {
        velocity += acceleration * dt;
    }
}

static void resolve_collisions(Vec2& position, Vec2& velocity, const SimSettings& settings) {
    const float half_width = settings.bounds_size.x * 0.5f;
    const float half_height = settings.bounds_size.y * 0.5f;

    if (std::fabs(position.x) >= half_width) {
        position.x = std::copysign(half_width, position.x);
        velocity.x *= -settings.collision_damping;
    }
    if (std::fabs(position.y) >= half_height) {
        position.y = std::copysign(half_height, position.y);
        velocity.y *= -settings.collision_damping;
    }
}

static void update_positions(Simulation& sim, const SimSettings& settings, float dt) {
    for (std::size_t i = 0; i < sim.positions.size(); i++) {
        sim.positions[i] += sim.velocities[i] * dt;
        resolve_collisions(sim.positions[i], sim.velocities[i], settings);
    }
}

void step_simulation(Simulation& sim, const SimSettings& settings, float dt) {
    apply_gravity(sim, settings, dt);
    calculate_densities(sim, settings);
    update_positions(sim, settings, dt);
}
```

**`static const float pi`** fayl boshida bir marta e'lon qilinadi, endi uni ham spawner, ham kernel ishlatadi. 04-darsdagi `two_pi` o'rniga spawner'da `2.0f * pi`.

**`density_kernel`** 3-bo'limdagi formulaning o'zi: radius tashqarisida 0, ichkarida `(r - d) × (r - d) × scale`. Kvadratlar oldindan solishtirilgani uchun bu
yerga faqat doira ichidagilar keladi, lekin funksiya o'z-o'zidan ham to'g'ri bo'lishi uchun tekshiruv qoldirilgan.

**`scale`** 4-bo'limdagi koeffitsient. U faqat radiusga bog'liq, shuning uchun har juftlik uchun emas, bir marta hisoblanadi. `radius * radius * radius * radius`
"radiusni to'rt marta o'ziga ko'paytirish".

**`const Vec2 p = sim.positions[i];`** `i` zarrachaning o'rnini bir marta olib qo'yamiz, ichki sikl uni 1148 marta o'qiydi.

**`density`** `float` o'zgaruvchida yig'iladi va sikl oxirida bir marta massivga yoziladi.

Qolgan funksiyalar 04-darsdagining o'zi.

### 9.3. `shaders/particle2d.vert` (to'liq almashtiring)

```glsl
#version 410 core

layout(location = 0) in vec2 in_position;
layout(location = 1) in float in_density;

uniform vec2 u_world_to_clip;
uniform float u_diameter;
uniform float u_reference_density;

out vec2 v_local;
out vec3 v_color;

const vec2 CORNERS[6] = vec2[6](
    vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0),
    vec2(-1.0, -1.0), vec2(1.0, 1.0), vec2(-1.0, 1.0)
);

const vec3 LOW_COLOR = vec3(0.15, 0.35, 0.90);
const vec3 MID_COLOR = vec3(0.95, 0.95, 0.95);
const vec3 HIGH_COLOR = vec3(0.90, 0.20, 0.15);

vec3 density_color(float density) {
    float t = density / u_reference_density;
    if (t < 1.0) {
        return mix(LOW_COLOR, MID_COLOR, t);
    }
    return mix(MID_COLOR, HIGH_COLOR, min(t - 1.0, 1.0));
}

void main() {
    vec2 corner = CORNERS[gl_VertexID];
    vec2 world = in_position + corner * 0.5 * u_diameter;
    gl_Position = vec4(world * u_world_to_clip, 0.0, 1.0);

    v_local = corner;
    v_color = density_color(in_density);
}
```

`in vec3 in_color` o'rniga `in float in_density` keldi. **`density_color`** shader ichidagi oddiy funksiya, C++ dagidek yoziladi. U `main` dan oldin turishi
kerak, chunki GLSL ham C kabi yuqoridan pastga o'qiladi. Ranglar `const` bo'lib, hamma vertex uchun bir xil. Fragment shader'ga tegmaymiz.

### 9.4. `src/main.cpp` (to'liq almashtiring)

```cpp
#include "gl.hpp"
#include "shader.hpp"
#include "sim2d.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdio>
#include <vector>

struct Controls {
    bool paused = false;
    bool step_once = false;
    bool reset = false;
};

static void on_glfw_error(int code, const char* description) {
    std::fprintf(stderr, "GLFW xatosi %d: %s\n", code, description);
}

static void on_key(GLFWwindow* window, int key, int, int action, int) {
    if (action != GLFW_PRESS) {
        return;
    }

    Controls* controls = static_cast<Controls*>(glfwGetWindowUserPointer(window));
    switch (key) {
    case GLFW_KEY_SPACE: controls->paused = !controls->paused; break;
    case GLFW_KEY_RIGHT: controls->step_once = true; break;
    case GLFW_KEY_R: controls->reset = true; break;
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
    default: break;
    }
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

    Controls controls;
    glfwSetWindowUserPointer(window, &controls);
    glfwSetKeyCallback(window, on_key);

    GLuint program = create_program("particle2d.vert", "particle2d.frag");
    if (!program) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    const GLint loc_world_to_clip = uniform_location(program, "u_world_to_clip");
    const GLint loc_diameter = uniform_location(program, "u_diameter");
    const GLint loc_reference_density = uniform_location(program, "u_reference_density");

    const SimSettings settings;
    const SpawnSettings spawn;
    Simulation initial = spawn_particles(spawn);
    calculate_densities(initial, settings);
    Simulation sim = initial;

    const float max_density = *std::max_element(initial.densities.begin(), initial.densities.end());
    std::printf("Zichlik: eng kattasi %.1f, spawner'da %.1f\n", max_density, spawn.density);

    const int particle_count = static_cast<int>(sim.positions.size());

    GLuint vao = 0;
    GLuint position_vbo = 0;
    GLuint density_vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &position_vbo);
    glGenBuffers(1, &density_vbo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), nullptr);
    glVertexAttribDivisor(0, 1);

    glBindBuffer(GL_ARRAY_BUFFER, density_vbo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);
    glVertexAttribDivisor(1, 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl_check("sozlash");

    double last_time = glfwGetTime();
    double fps_time = last_time;
    int fps_frames = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        const double now = glfwGetTime();
        const float frame_time = static_cast<float>(now - last_time);
        last_time = now;

        if (controls.reset) {
            sim = initial;
            controls.reset = false;
            controls.paused = true;
        }

        if (!controls.paused || controls.step_once) {
            const float dt = std::min(frame_time, settings.max_frame_time) / settings.iterations_per_frame;
            for (int i = 0; i < settings.iterations_per_frame; i++) {
                step_simulation(sim, settings, dt);
            }
        }
        controls.step_once = false;

        int fb_width = 0;
        int fb_height = 0;
        glfwGetFramebufferSize(window, &fb_width, &fb_height);
        glViewport(0, 0, fb_width, fb_height);

        glClearColor(0.05f, 0.07f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        const float aspect = static_cast<float>(std::max(fb_width, 1)) / std::max(fb_height, 1);
        float half_height = 5.0f;
        float half_width = half_height * aspect;

        const float min_half_width = settings.bounds_size.x * 0.5f * 1.02f;
        if (half_width < min_half_width) {
            half_width = min_half_width;
            half_height = half_width / aspect;
        }

        glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
        glBufferData(GL_ARRAY_BUFFER, sim.positions.size() * sizeof(Vec2), sim.positions.data(), GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, density_vbo);
        glBufferData(GL_ARRAY_BUFFER, sim.densities.size() * sizeof(float), sim.densities.data(), GL_STREAM_DRAW);

        glUseProgram(program);
        glUniform2f(loc_world_to_clip, 1.0f / half_width, 1.0f / half_height);
        glUniform1f(loc_diameter, 0.08f);
        glUniform1f(loc_reference_density, spawn.density);

        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, particle_count);
        gl_check("kadr");

        glfwSwapBuffers(window);

        fps_frames++;
        if (now - fps_time >= 0.5) {
            int win_width = 0;
            int win_height = 0;
            glfwGetWindowSize(window, &win_width, &win_height);

            char title[128];
            std::snprintf(title, sizeof(title), "Fluid Sim | %.0f FPS | %d zarracha | oyna %dx%d | framebuffer %dx%d%s",
                          fps_frames / (now - fps_time), particle_count, win_width, win_height, fb_width, fb_height,
                          controls.paused ? " | pauza" : "");
            glfwSetWindowTitle(window, title);

            fps_time = now;
            fps_frames = 0;
        }
    }

    glDeleteBuffers(1, &position_vbo);
    glDeleteBuffers(1, &density_vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

04-darsdan farqlari:

- `struct Color` va `make_colors` olib tashlandi, rangni endi shader hisoblaydi.
- `color_vbo` o'rnida `density_vbo`. Unga sikldan oldin ma'lumot yozilmaydi, xuddi `position_vbo` kabi har kadrda `GL_STREAM_DRAW` bilan yangilanadi, chunki
  zichlik ham har qadamda o'zgaradi.
- Yangi uniform `u_reference_density`, kadr ichida `spawn.density` (159) beriladi.
- `initial` uchun `calculate_densities` va terminalga tekshiruv qatori (8-bo'lim).

### 9.5. Yig'ish va ishga tushirish

```
cmake --build build
.\build\fluid_sim.exe      # Windows (PowerShell)
./build/fluid_sim          # macOS
```

## 10. Ishlaganini qanday tekshirasiz

**Windows.** Bu natijani o'zim ko'rdim (1280×720, RTX 3060):

- Terminalda: `Zichlik: eng kattasi 160.1, spawner'da 159.0`.
- Boshida blok 04-darsdagidan kichik (237 × 164 piksel). Ichi deyarli oq, chetlari och ko'k, burchaklari ko'kroq.
- Blok yaxlit tushayotganda ranglar o'zgarmaydi: zarrachalar orasidagi masofa o'zgarmayapti, demak zichlik ham.
- Pastki qatorlar polga urilib qaytganda, yuqoridan tushayotgan qatorlar bilan ustma-ust tushadi: o'sha joy qizil bo'ladi. Ajralib qolgan siyrak qatorlar ko'k.
- Taxminan 36 soniyada hamma 1148 zarracha polda, chiziq qizil (o'rtacha zichlik 1255, me'yordan 8 baravar ko'p). 06-darsdagi bosim aynan shunga yo'l qo'ymaydi.
- R: boshlang'ich blok o'z ranglari bilan va pauza. Sarlavhada 165 FPS, terminalda `OpenGL xatosi` yo'q.

**macOS.** Buni o'zim ishga tushira olmadim, kutilayotgan natija: xuddi shu ranglar va harakat. Terminaldagi son biroz boshqacha bo'lishi mumkin (04-dars, jitter
kutubxonaga bog'liq), lekin 155–165 oralig'ida bo'lishi kerak.

**Nima buzilsa nima bo'ladi.** OpenGL qatorlari RTX 3060 da ishga tushirib ko'rilgan, oxirgi ikki qator dasturda hisoblab tekshirilgan:

| O'zgarish | Natija | Xabar bormi |
|---|---|---|
| `scale = 1.0f` (normalizatsiya yo'q) | hamma zarracha ko'k: zichlik 1.2 atrofida | yo'q |
| `scale` da radius ikki marta (`pi * radius * radius`) | hamma zarracha ko'k: zichlik 19 atrofida | yo'q |
| `densities.resize` yo'q | dastur darhol yiqiladi (`0xC0000005`) | yo'q |
| `glUniform1f(loc_reference_density, ...)` yo'q | hamma zarracha qizil: uniform 0 bo'lib qoladi, 0 ga bo'linadi | yo'q |
| `calculate_densities(initial, ...)` yo'q | R bosilganda hamma zarracha ko'k | yo'q |
| `density_vbo` qadami `sizeof(Vec2)` | ranglar aralashib ketadi: zarrachalar boshqalarning zichligini, yarmi esa bufer tashqarisini o'qiydi | yo'q |
| `i` ning o'zi hisobga olinmasa | blok ichida 155 o'rniga 139, rang ko'kroq | yo'q |
| har juftlik uchun `sqrt` | rasm bir xil, hisob 1.3–1.5 baravar sekin | yo'q |

Uniform qatoriga e'tibor bering: `uniform_location` nomni topdi, demak terminal jim. Qiymat berilmagan uniform 0 ga teng bo'ladi.

## 11. Git va Mac

Windows'da:

```
git add .
git commit -m "Lesson 05: density with a smoothing kernel"
git push
```

Mac'da:

```
git pull
cmake --build build
./build/fluid_sim
```

## 12. Ixtiyoriy tajribalar

1. `sim2d.hpp` da `smoothing_radius = 0.5f` qiling: blok ichi baribir oq (normalizatsiya ishlayapti), lekin ko'k chet kengroq bo'ladi, chunki doira chetdan uzoqroqqa yetadi.
   Keyin `0.2f` qilib, ichkaridagi ranglar notekisroq bo'lishini kuzating (4-bo'limdagi jadval).
2. `size = {6.42f, 4.39f}` qilib, to'liq blokni qaytaring. Sarlavhada FPS tushganini va harakat sekinlashganini ko'rasiz (6-bo'lim). Mac'da qancha FPS chiqishini yozing.

## 13. Lug'at

| So'z | Ma'nosi |
|---|---|
| zichlik (density) | bir kvadrat birlikka qancha massa to'g'ri keladi |
| kvadrat birlik | tomoni 1 bo'lgan kvadratning yuzi |
| yuz (area) | shaklning tekislikda egallagan joyi, to'rtburchakda eni × bo'yi |
| pi | aylana uzunligining diametrga nisbati, 3.14159... |
| doira yuzi | pi × radius × radius |
| aylana uzunligi | 2 × pi × radius |
| SPH | suyuqlikni zarrachalar va silliqlovchi kernel bilan hisoblash usuli |
| smoothing kernel | masofaga qarab qo'shniga vazn beradigan qoida, chetda 0 |
| vazn (weight) | qo'shnining hisobga qancha ta'sir qilishi |
| hajm | shakl ichiga qancha narsa sig'ishi, qutida eni × bo'yi × balandligi |
| normalizatsiya | kernel tepaligining hajmini 1 ga keltirish |
| integral | juda ko'p ingichka bo'laklarni qo'shib, aniq hajm yoki yuzni topish |
| Pifagor teoremasi | to'g'ri burchakli uchburchakda masofa × masofa = dx × dx + dy × dy |
| O(n²) | ish zarrachalar sonining kvadratiga qarab o'sadi |
| `mix(a, b, t)` | `a` va `b` orasidagi `t` ulushdagi qiymat |

## Keyingi dars

06-darsda zichlik **bosimga** aylanadi: zichlik me'yoridan katta bo'lgan joyda zarrachalar bir-birini itaradi, kichik joyda esa tortadi. Polda chiziq o'rniga birinchi marta
haqiqiy suyuqlik ko'rinadi. Buning uchun kernel'ning qiyaligi (u masofa bilan qanchalik tez o'zgarishi) kerak bo'ladi, uni ham noldan tushuntiramiz.

Shu dars tugagach yozing. Xohlasangiz, kodingizni ko'rib chiqaman.
