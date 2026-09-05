<!-- generated-by: ai-github-projects/tools/aar_context.py -->

# CONTEXT.md — `swe-android-lib`

> База знань про цю AAR-бібліотеку: що вона дає застосунку і як нею
> користуватись. Розділи, позначені **GENERATED**, пише
> `ai-github-projects/tools/aar_context.py` з реальних файлів — правити їх
> руками немає сенсу, вони перезапишуться. Проза живе в
> `tools/aar_context_prose.py`.

## Коротко

**Swiss Ephemeris як нативна бібліотека** — `libswe-<версія>.so` під чотири ABI.
Java-декларації 106 нативних методів приходять окремо, Maven-артефактом `swisseph:swe-api`
(через `swisseph:swe` / `swisseph:swe-jyotisa`).

Ефемеридні дані (`*.se1`) тут **не** лежать — вони в `jyotisa-assets`.

## Підключити

```gradle
implementation 'swisseph:swe-android:2.10.03j6f'
```

## Що дає застосунку

`swisseph.SwephExp` (з `swe-api`) після `SwephExp.loadSweLibrary()`; практично —
через `org.swisseph.SwephNative` і далі `EpheManager`.

## Залежності — GENERATED

Жодних — самодостатня.

## Вміст — GENERATED

| що | скільки |
|---|---|
| `jniLibs/arm64-v8a` | libswe-2.10.03j6f.so |
| `jniLibs/armeabi-v7a` | libswe-2.10.03j6f.so |
| `jniLibs/x86` | libswe-2.10.03j6f.so |
| `jniLibs/x86_64` | libswe-2.10.03j6f.so |

## Java-поверхня — GENERATED

_Java-коду немає._ Це чисто ресурсна/асетна бібліотека.

## Пастки

* **Версія `.so` мусить збігатися з `SWISSEPH_LIBRARY_NAME`** у `swe-api`,
  інакше `UnsatisfiedLinkError` у момент виклику, а не завантаження. Повний список із девʼяти
  місць, де записана версія, — у `ai-github-projects/CLAUDE.md`.
* `swed` — **thread-local**, тож `swe_set_ephe_path()` треба викликати на кожному потоці.
* `arm64-v8a` контрактує множення-додавання у FMA і **не дає побітово ті самі числа**, що інші
  три ABI (~1e-11°). Це задокументовано, не вада.

## Координати й оточення — GENERATED

|  |  |
|---|---|
| Maven | `swisseph:swe-android:2.10.03j6f` |
| namespace | `org.swisseph` |
| SDK | compile 37, min 21, target 36 |
| AndroidManifest | порожній `<manifest/>` |
| дозволи | жодного |
| хто вже використовує | `22-energies`, `22-energies-apac`, `22-energies-emea`, `22-energies-nam`, `22-energies-oppo`, `22-energies-ovxi`, `22-energies-ru`, `jyotisa-basics` |

## Збірка

Однакова для всіх 24 AAR-бібліотек — еталон `holidates-suppart`, уся метадата в
`gradle.properties` (`AAR_*`). Перезібрати й опублікувати **все** в правильному порядку:

```bash
cd /e/Softworks/Holidates/ai-github-projects/tools
"$PYTHON_HOME/python.exe" aar_publish.py --all
```

Порядок рахується з `dependencies`, бо через `mavenLocal()` публікація в неправильному порядку
дає збірку, що мовчки підтягла несвіжу залежність. Подробиці —
`ai-github-projects/tools/README-aar.md`, карта стеку — `ai-github-projects/tools/AAR-STACK.md`.
