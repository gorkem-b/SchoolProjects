**Kullanılacak Tech-Stack**
* **Programlama Dili:** C++ (C++11 veya üzeri)
* **Grafik API:** OpenGL (Fixed Function Pipeline / `glPushMatrix`, `glPopMatrix` uyumluluğu için)
* **Pencere ve Girdi Yönetimi:** FreeGLUT veya GLFW
* **Derleme Sistemi:** CMake
* **Matematik Kütüphanesi:** OpenGL dahili matris işlemleri (veya modern yaklaşıma geçilecekse GLM)

---

**Projenin Tamamlandığında Çalışma Şekli**
Program çalıştırıldığında, ekranın merkezinde 3 boyutlu koordinat sistemi üzerinde (tercihen bir referans ızgarası/grid üzerinde) duran hiyerarşik bir mekanik figür oluşturulur. Sistem saniyede hedeflenen kare hızında (örn. 60 FPS) sürekli olarak kendini güncelleyen bir döngüde (Main Loop) çalışır.

Kullanıcı klavye tuşlarına (örn. omuz için 'Q/A', dirsek için 'W/S') bastığında, girdiler olay dinleyicileri (event listeners) tarafından yakalanır. Bu girdiler, sistemin belleğinde tutulan eklem açılarını (state) doğrudan değiştirir. Açıların değişmesiyle birlikte render motoru, hiyerarşik matris çarpımlarını yeniden hesaplar. Gövde sabit kalsa da, matris yığını (Matrix Stack) mantığı sayesinde omuzun dönüşü alt kolu ve eli matematiksel olarak doğru yörüngede uzayda kaydırır. Kullanıcı farenin tuşlarına basılı tutarak kamerayı sahne etrafında döndürebilir (Orbit) ve derinlik testine (Z-Buffer) tabi tutulmuş eksiksiz bir 3D simülasyon izler.

---

**Sistem Tasarımı ve Mimari**
Mimari, "State-Driven Rendering" (Durum Odaklı Çizim) ve "Scene Graph" (Sahne Grafiği) prensiplerinin basitleştirilmiş bir versiyonuna dayanır. 

* **Veri Akış Mimarisi (Data Flow):** Sistem tek yönlü bir veri akışına sahiptir: `Girdi (Input) -> Durum Güncellemesi (State Update) -> Çizim (Render)`. Bu ayrım, kodun spagettiye dönüşmesini engeller.
* **Hiyerarşik Tasarım (Scene Graph):** Figürün uzuvları bağımsız objeler değildir. Ana gövde "Kök Düğüm" (Root Node), omuzlar onun "Çocuk Düğümleri" (Child Nodes), dirsekler ise omuzların çocuklarıdır. Her düğüm kendi lokal koordinat sistemine (Local Space) sahiptir.
* **Matris Yığını Mimarisi (Matrix Stack):** OpenGL'in `glPushMatrix()` (Mevcut durumu kaydet) ve `glPopMatrix()` (Eski duruma dön) fonksiyonları kullanılarak ağaç yapısı üzerinde Derinlik Öncelikli Arama (DFS - Depth First Search) benzeri bir gezinme ile çizim yapılır. Ana koordinat sistemi bozulmadan uç noktalara kadar ilerlenir ve geri dönülür.

---

**Akış Diyagramı**

```text
[Başlat]
   │
   ▼
[Sistem İlklendirme (Init)] ───────> OpenGL Context, Z-Buffer Aktifleşimi, Işık/Renk Ayarları
   │
   ▼
[Ana Döngü (Main Loop)] ◀─────────────────────────┐
   │                                              │
   ├──> [Girdi Bekleme (Idle/Event Polling)]      │
   │        │                                     │
   │        ▼                                     │
   │    [Girdi İşleme (Input Handler)]            │
   │        ├──> Tuş kontrolü (Klavye)            │
   │        └──> Kamera kontrolü (Fare)           │
   │        │                                     │
   │        ▼                                     │
   │    [Durum Güncelleme (State Update)]         │
   │        └──> Eklem açılarını (+/-) değiştir   │
   │        │                                     │
   │        ▼                                     │
   │    [Ekran Yenileme Tetikleyicisi]            │
   │        └──> glutPostRedisplay()              │
   │                                              │
   ▼                                              │
[Render (Display Fonksiyonu)]                     │
   │                                              │
   ├──> 1. Ekranı Temizle (Color & Depth Bit)     │
   ├──> 2. Kamera Matrisini Uygula (gluLookAt)    │
   ├──> 3. Çevre Bileşenlerini Çiz (Grid)         │
   │                                              │
   ├──> 4. [Sahne Grafiği İşleme]                 │
   │        ├──> Gövde Çiz                        │
   │        ├──> glPushMatrix()                   │
   │        ├──> Omuza Translate -> Rotate        │
   │        ├──> Omuz Çiz                         │
   │        ├──> glPushMatrix()                   │
   │        ├──> Dirseğe Translate -> Rotate      │
   │        ├──> Dirsek Çiz                       │
   │        ├──> glPopMatrix() (Dirsekten çık)    │
   │        └──> glPopMatrix() (Omuzdan çık)      │
   │                                              │
   └──> 5. Çift Tampon Takası (Swap Buffers) ─────┘
```

---

**Klasör Yapısı ve Dosya Sorumlulukları**

```text
/3DKinematicRobot
├── CMakeLists.txt       # Projenin derleme konfigürasyon dosyası. Kütüphane bağlamaları burada yapılır.
├── /build               # Derlenmiş çalıştırılabilir dosyaların (executable) oluşturulduğu dizin. (Git'e eklenmez)
├── /lib                 # Projeye özel dış kütüphaneler (gerekiyorsa).
└── /src                 # Kaynak kod dizini
    ├── main.cpp         # Uygulamanın giriş noktası. Pencere oluşturma ve ana döngüyü (main loop) başlatma.
    ├── App.cpp          # OpenGL bağlamının ilklendirilmesi ve genel uygulama döngüsünün yönetimi.
    ├── App.h            # App sınıfının tanımlamaları.
    ├── Robot.cpp        # Hiyerarşik çizim mantığının, push/pop matris işlemlerinin ve geometri çizimlerinin yapıldığı dosya.
    ├── Robot.h          # Robot durum değişkenlerinin (açılar) ve fonksiyon prototiplerinin bulunduğu dosya.
    ├── Camera.cpp       # gluLookAt yönetimini, fare hareketlerine göre kameranın dönüşünü (Pitch, Yaw) hesaplayan dosya.
    ├── Camera.h         # Kamera sınıfının tanımlamaları.
    ├── Input.cpp        # Klavye (keyboard) ve fare (mouse) geri çağırım (callback) fonksiyonlarını barındırır.
    └── Input.h          # Input yönetim fonksiyonlarının tanımlamaları.
```

**Dosya Sorumluluk Dağılımı:**
* **`main.cpp`:** Sadece `App` nesnesini yaratır ve çalıştırır. İçinde OpenGL çizim kodu bulunmaz.
* **`App.cpp`:** `Robot` ve `Camera` nesnelerini bir araya getirir. `display()`, `reshape()` gibi ana OpenGL döngülerini yönetir.
* **`Robot.cpp`:** Sistemin "Görünüm" (View) ve "Model" (Model) kısmıdır. Sadece çizim işine ve matris matematiğine odaklanır. Açı parametrelerini dışarıdan alır.
* **`Input.cpp`:** Sistemin "Kontrolcü" (Controller) kısmıdır. Fiziksel donanımdan gelen veriyi mantıksal komutlara çevirir ve `Robot` objesinin durum değişkenlerini günceller.

---

**Doğrusal Geliştirme Adımları**

1.  **Kurulum ve Derleme Sistemi İnşası:**
    * CMakeLists.txt dosyasını oluştur. OpenGL, GLU ve FreeGLUT kütüphanelerini projeye bağla.
    * Derleme işleminin (build) başarılı olduğunu test etmek için boş bir `main.cpp` oluştur.
2.  **Pencere ve Bağlam (Context) Oluşturma (`App.cpp`):**
    * Pencere boyutlarını belirle.
    * Boş ekranı siyah/gri renkle temizleyen `display` fonksiyonunu yaz. Çift tamponlamayı (Double Buffering) aktif et.
3.  **Kamera ve Derinlik Sisteminin Kurulumu (`Camera.cpp`):**
    * Z-Buffer'ı aktif et (`glEnable(GL_DEPTH_TEST)`).
    * Perspektif projeksiyonu ayarla (`gluPerspective`).
    * Kamerayı sahnede çapraz bir noktaya yerleştir (`gluLookAt`).
    * Referans olması için yere bir grid (ızgara) çizen temel bir fonksiyon ekle.
4.  **Durum (State) Değişkenlerinin Eklenmesi (`Robot.h`):**
    * Omuz, dirsek, kalça, diz gibi eklemler için `float` cinsinden açı değişkenlerini sıfır (0.0f) olarak tanımla.
5.  **Kök Geometrinin (Gövde) Çizilmesi (`Robot.cpp`):**
    * Merkez (0,0,0) noktasına `glutSolidCube` veya `glutWireCube` kullanarak sadece gövdeyi çiz.
6.  **Hiyerarşik Çizim Algoritmasının (Kol) Uygulanması:**
    * Gövdeden sonra `glPushMatrix` çağır.
    * `glTranslatef` ile çizim noktasını omuz pivot noktasına kaydır.
    * Omuz açı değişkenini kullanarak `glRotatef` çağır. Üst kolu çiz.
    * Tekrar `glPushMatrix` çağır.
    * Alt kol için dirsek noktasına translate ve rotate işlemi yap. Alt kolu çiz.
    * İki kez `glPopMatrix` çağırarak ana koordinata dön.
7.  **Girdi ve Etkileşim Entegrasyonu (`Input.cpp`):**
    * Klavye geri çağırım (callback) fonksiyonunu yaz.
    * Tuş basımlarını `Robot` objesindeki açı değişkenlerine bağla (Örn: 'O' tuşu omuz açısını +5 artırır).
    * Her tuş basımı sonunda ekranın yeniden çizilmesi için `glutPostRedisplay()` tetiklemesini ayarla.
8.  **Test ve Hata Ayıklama:**
    * Dönüşlerin (rotasyon) yanlış merkezden (pivot) olup olmadığını test et. Objeler kendi merkezlerinden dönüyorsa, `rotate` işleminden önce lokal bir `translate` ötelemesi ekleyerek pivot noktasını eklem yerlerine çek.
9.  **Genişletme (Deploy/Finalization):**
    * Aynı mantığı kullanarak diğer kolu, bacakları ve kafayı hiyerarşiye ekle.
    * Işıklandırma (`GL_LIGHTING`) ekleyerek materyallerin 3D algısını artır. Kaynak kodunu derle ve executable dosyayı oluştur.