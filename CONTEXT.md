# CONTEXT.md — стан робіт swe-android-lib

Стисло: що зроблено, що відкрито. Факт, що змінився, замінювати, а не дописувати. Спільні для
обох Android-портів речі (джерело виправлення, скрипти перевірки) — у
`swisseph-2026/CONTEXT.md`.

Стан на 21.10.2026.

## Що це

AAR з нативною Swiss Ephemeris 2.10.03 (модуль `swe-2.10.03j6f`, чотири ABI) і JNI-обгорткою
`jni/swejni.c` / `jni/swejni.h` для `swisseph.SwephExp` з `swe-java-api`. Збірка —
`ndk-build` (NDK) у корені, далі gradle.

## Стан `jni/`

* `sweph.c` / `sweph.h` = `swisseph-2026` (upstream `c69c14a` + `segfix.patch`), решта файлів
  бібліотеки = upstream `c69c14a` (коміти `c0e554a`, `45de244`, запушено). Перевірка —
  `python tools/jni_status.py <swe-android-lib>/jni` у swisseph-2026.
* TLS-виправлення для MinGW (`sweodef.h`) тут не потрібне: NDK clang не визначає ні `WIN32`, ні
  `__MINGW32__`, тож TLS — `__thread`.

## Опції збірки (21.10, коміт `42c9db6`)

* `jni/Application.mk`: `APP_PLATFORM := android-24` (було 21). `swejpl.c` вмикає
  `_FILE_OFFSET_BITS 64`, а bionic на 32-бітних ABI оголошує `fseeko`/`ftello` лише з API 24; за
  API 21 це були неявні оголошення, і на armeabi-v7a/x86 кожне позиціювання в JPL-файлі було
  хибним (`.se1` не торкалося). Тепер `.so` не вантажиться нижче API 24.
* `jni/Android.mk`: прибрано `-Wno-error=implicit-function-declaration` (воно цю ваду ховало) і
  `-ffunction-sections -fdata-sections` з `LOCAL_LDFLAGS` (на лінкуванні нічого не дають).
* `gradle.properties`: `AAR_MIN_SDK=24` — під `APP_PLATFORM`.
* Перевірка компіляції чотирьох ABI —
  `python tools/android/ndk_compile_check.py <swe-android-lib>/jni` у swisseph-2026.

## Огляд `swejni.c` / `swejni.h` (21.10) — правки ще не внесено

Сигнатури узгоджені: `python tools/android/jni_check.py <swe-android-lib>/jni
<swe-java-api>/src/main/java/swisseph/SwephExp.java` (у swisseph-2026) — Java `native`
106 = `.h` 106 = `.c` 106, проблем 0; `arm64-v8a/*.so` експортує ті самі 106 `Java_*`.

Що виправити, за вагою:

1. **Довжини Java-масивів не перевіряються.** `GET_DOUBLE_ARRAY_ELEMENTS` (`swejni.c:33`) віддає
   вказівник у swisseph без `GetArrayLength`; закороткий масив = запис за межі в купу Java
   (псування пам'яті, падіння деінде). Мінімуми з коду swisseph:

   | функція | масив | мінімум |
   |---|---|---|
   | `calc*`, `fixstar*`, `nod_aps*` (вихідні) | `xx` | 6 |
   | `houses*` | `cusps` / `ascmc` | 13, для `'G'` — **37** (swehouse.c:1624) / 10 |
   | `sol_eclipse_where`, `lun_occult_where` | `geopos` / `attr` | **10** (документація; код пише щонайменше `[2]`) / **20** (swecl.c:3257, 3806) |
   | `*_when_*` (затемнення) | `tret` | 10 (swecl.c:1234, 1634, 2441, 3421) |
   | `*_how`, `*_when_loc` | `attr` | 20 |
   | `get_orbital_elements` | `dret` | **50** (swecl.c:5752, «declare as dret[50]») |
   | `heliacal_ut` | `dret` / `datm` / `dobs` | ≥ 9 (документація — 50) / 4 / 6 — `datm`, `dobs` swisseph **перезаписує** типовими |
   | `heliacal_pheno_ut` | `darr` | 30 (документація — 50) |

   Стосується обгорток, що передають масив прямо: `calc*`, `fixstar*`, `houses*`, затемнення,
   `pheno*`, `nod_aps*`, `get_orbital_elements`, `heliacal*`, `utc_to_jd`, `azalt*`, `cotrans*`,
   `mooncross_node*`, `helio_cross*`, `get_ayanamsa_ex*`, `date_conversion`, `time_equ`,
   `lmt_to_lat`/`lat_to_lmt`, `gauquelin_sector`. Через `SET_*_ARRAY_ELEMENTS` (з урахуванням
   довжини) вже йдуть `revjul`, `jdet_to_utc`, `jdut1_to_utc`, `utc_time_zone`, `split_deg`.
   Задум: макрос `GET_…_MIN(arr, n)` — за нестачі `IllegalArgumentException` і `ERR` без виклику
   swisseph.
2. **`null`-масив -> SIGSEGV**: `null` стає `NULL`, swisseph розіменовує. Дозволено лише
   `cusp_speed`/`ascmc_speed` у `houses_ex2` (swehouse.c:644/646/685). Непослідовно в
   `swe_get_current_file_data` (`swejni.c:807–822`): `denum` захищено, `tfstart`/`tfend` — ні.
3. **Стан swisseph — свій у кожного потоку** (`TLS __thread`): шлях ефемерид, `sid_mode`, `topo`,
   відкриті `FILE*`. Виклики з різних потоків (пул, корутини) потребують у кожному
   `swe_set_ephe_path`/`swe_set_sid_mode`/`swe_set_topo`, інакше тихий перехід на Moshier; файли
   потоку, що завершився, лишаються відкритими. Задокументувати: один потік або `swe_close()`
   наприкінці роботи потоку.
4. **`ERR_BUILDER_APPEND_IF_ERR` губить попередження** (`swejni.c:30`): лише `retc <= ERR`, а
   swisseph пише `serr` і за `retc >= 0` — головне, перехід `swe_calc*` на Moshier без `.se1`.
   `IF_SERR` є лише в `houses_ex2`, `houses_armc_ex2`, `house_pos`, `deltat_ex`, `solcross*`,
   `mooncross*`, `helio_cross*`; для `calc`/`calc_ut`/`fixstar*` перейти на `IF_SERR`.
5. **`NewStringUTF` чекає modified UTF-8**: назви зірок/астероїдів із `sefstars.txt` /
   `seasnam.txt` і шляхи з байтами > 0x7F -> `abort` під CheckJNI (debug), сміття в release.
   `swejni.c:174` (`appendToBuilder`), 698, 792, 814, 1118.
6. **Немає обгортки `swe_get_denum`** (`swephexp.h:697`, нове в `c69c14a`) — єдиний експорт без
   неї; потрібна в `SwephExp.java` + `.h` + `.c`, якщо показувати номер DE.
7. Дрібне:
   * `getBuilderString`/`emptyBuilder`/`appendToBuilder` не `static`, на кожен виклик
     `GetObjectClass` + `GetMethodID` — можна кешувати в `JNI_OnLoad`.
   * Після `CallObjectMethod`/`NewStringUTF` не перевіряється виняток (`OutOfMemoryError`).
   * `RLZ_DOUBLE_ARRAY_ELEMENTS_OK` = `RLZ_DOUBLE_ARRAY_ELEMENTS` (`swejni.c:37`, `41`).
   * `RLZ_*` звільняє з режимом 0 (копіює назад) і вхідні масиви — правильно для `datm`/`dobs`,
     для решти зайве копіювання, якщо ART віддав копію.
   * `cs2lonlatstr` (`swejni.c:2262`): `(char) jchar` обрізає символ поза ASCII.
   * `BUILDER_APPEND_IF_DIFF` (`swejni.c:101`): ім'я довше за `AS_MAXCH` обрізає `strncpy`,
     `strcmp` бачить «відмінність» — builder отримує обрізане ім'я замість повного.
   * `set_ephe_path`/`set_jpl_file` (`swejni.c:668`) віддають `GetStringUTFChars` прямо: від
     2.10.03 swisseph копіює рядок, тож безпечно.

## Відкрито

* Огляд `swejni.c` вище — правки не внесено («потім зробимо», 21.10).
* Інструментальні тести на пристрої не запускались.
