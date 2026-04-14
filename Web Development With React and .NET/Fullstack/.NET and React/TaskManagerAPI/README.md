### Proje İşleyişi

Kullanıcı React tabanlı arayüze erişerek kayıt olur veya mevcut bilgileriyle giriş yapar. Giriş başarılı olduğunda arka uç (.NET API), kullanıcıya özgü şifrelenmiş bir JSON Web Token (JWT) üretir ve arayüze döndürür. React uygulaması bu token'ı tarayıcı belleğinde (localStorage veya sessionStorage) saklar. Kullanıcı yeni bir görev ekleme, silme, güncelleme veya görevlerini filtreleyerek listeleme talebinde bulunduğunda, React uygulaması bu JWT'yi HTTP isteklerinin `Authorization` başlığına (Bearer token olarak) ekleyerek API'ye iletir. API, gelen istekteki token'ın geçerliliğini ve kullanıcının kimliğini doğrular. Doğrulama başarılıysa, MSSQL veritabanında kullanıcının yalnızca kendi verileri üzerinde ilgili CRUD (Oluşturma, Okuma, Güncelleme, Silme) işlemini gerçekleştirir. İşlem sonucu, JSON veri formatında ve ilgili HTTP durum kodlarıyla (örn. 200 OK, 201 Created) arayüze geri döndürülür ve arayüzdeki durum (state) güncellenerek ekrana yansıtılır.

---

### Tech-Stack

* **Frontend:** React (UI kütüphanesi), Axios veya Fetch API (HTTP istemcisi), React Router DOM (Sayfa yönlendirmeleri).
* **Backend:** C# ASP.NET Web API (RESTful mimari).
* **Database:** MSSQL (İlişkisel veritabanı).
* **ORM (Object-Relational Mapping):** Entity Framework (C# nesneleri ile MSSQL tabloları arasındaki eşleme).
* **Security/Authentication:** JWT (JSON Web Token).

---

### Sistem Tasarımı ve Mimari

Uygulama, birbirinden bağımsız çalışan İstemci-Sunucu (Client-Server) mimarisi üzerine kuruludur. Frontend (React) ve Backend (.NET) farklı sunucularda barındırılabilir, birbirleriyle sadece HTTP protokolü ve JSON verisi üzerinden haberleşirler.

Backend, **N-Katmanlı (N-Tier) Mimari** prensiplerine göre inşa edilir. Bu, kodun sorumluluklara göre ayrılmasını sağlar:
1.  **Sunum Katmanı (Controllers):** Sadece gelen HTTP isteklerini karşılar, verinin formatını doğrular ve iş katmanına iletir. Veritabanı sorgusu içermez.
2.  **İş Katmanı (Services):** Uygulamanın mantığının çalıştığı yerdir. Şifre hashleme, token üretme, bir görevin sadece o görevi oluşturan kullanıcı tarafından silinebilmesi gibi kurallar burada işletilir.
3.  **Veri Erişim Katmanı (Data Access / Repository):** Sadece veritabanı (MSSQL) ile konuşur. EF Core kullanılarak sorgular atılır ve sonuçlar nesne (Object) olarak iş katmanına döndürülür.

---

### Akış Diyagramı (Görselleştirme)

```text
[KULLANICI] 
    │
    ▼ (Arayüz Etkileşimi)
[REACT UI]
    │
    ├─► 1. POST /api/auth/login (Kullanıcı adı ve şifre gönderilir)
    │
    ▼
[.NET API CONTROLLER] ──► Doğrulama (Validation)
    │
    ▼
[AUTH SERVICE] ──► Şifre kontrolü ve UserID tespiti
    │
    ▼
[MSSQL DATABASE] ──► Kullanıcı kaydını sorgular ve onaylar
    │
    ▼
[AUTH SERVICE] ──► UserID içeren bir JWT oluşturur
    │
    ▼
[.NET API CONTROLLER] ──► HTTP 200 OK ve JWT döndürür
    │
    ▼
[REACT UI] ──► JWT'yi LocalStorage'a kaydeder.
    │
    ├─► 2. GET /api/tasks (Header: "Authorization: Bearer <JWT>")
    │
    ▼
[.NET API CONTROLLER] ──► Token'ı çözer, UserID'yi çıkarır, Yetkilendirmeyi (Auth) onaylar
    │
    ▼
[TASK SERVICE] ──► İlgili UserID'ye ait görevleri getir kuralını uygular
    │
    ▼
[MSSQL DATABASE] ──► "SELECT * FROM Tasks WHERE UserId = @UserId" çalıştırır
    │
    ▼
[TASK SERVICE] ──► Veritabanı modellerini DTO (Data Transfer Object) formatına çevirir
    │
    ▼
[.NET API CONTROLLER] ──► HTTP 200 OK ve JSON formatında görev listesini döndürür
    │
    ▼
[REACT UI] ──► State güncellenir ve görevler ekranda listelenir
```

---

### Klasör Yapısı Ağacı (Tree) ve Sorumluluklar

#### 1. Backend (.NET API) Klasör Yapısı

```text
TaskApi/
├── Controllers/
│   ├── AuthController.cs     # Login ve Register HTTP endpointlerini (POST) içerir.
│   └── TaskController.cs     # Task CRUD (GET, POST, PUT, DELETE) endpointlerini içerir.
├── Models/                   # Veritabanı tablolarının birebir C# sınıfı karşılıkları (Entities).
│   ├── User.cs               # Id, Username, PasswordHash alanları.
│   └── TaskItem.cs           # Id, Title, Description, Category, Status, UserId alanları.
├── DTOs/                     # İstemciye giden ve gelen veri şablonları. Güvenlik ve gereksiz veri transferini önler.
│   ├── UserAuthDto.cs        # Sadece Username ve Password taşır.
│   └── TaskResponseDto.cs    # Görev detaylarını taşır, ancak User tablosuna ait ilişkisel gizli verileri içermez.
├── Services/                 # İş kurallarının barındığı servis sınıfları.
│   ├── AuthService.cs        # JWT üretimi ve şifre doğrulama (BCrypt/Argon2) işlemleri.
│   └── TaskService.cs        # Task verilerini filtreleme, ekleme sırasında UserId atama işlemleri.
├── Data/
│   └── AppDbContext.cs       # Entity Framework'ün veritabanı bağlantı konfigürasyonu ve tablo setleri (DbSet).
├── Migrations/               # Veritabanı şemasında yapılan değişikliklerin otomatik oluşturulan tarihçesi.
├── appsettings.json          # Veritabanı Connection String'i ve JWT Secret Key'inin tutulduğu konfigürasyon dosyası.
└── Program.cs                # Uygulamanın başlangıç noktası. Bağımlılık enjeksiyonları (DI), CORS ayarları ve Middleware (JWT doğrulama) eklemeleri yapılır.
```

#### 2. Frontend (React) Klasör Yapısı

```text
task-ui/
├── public/
│   └── index.html            # React uygulamasının gömüldüğü ana HTML dosyası.
├── src/
│   ├── api/
│   │   ├── axiosConfig.js    # Base URL tanımı ve her giden isteğe JWT ekleyen Axios Interceptor yapısı.
│   │   └── taskServices.js   # API'ye yapılan /tasks endpoint isteklerini (fetch/axios) soyutlayan fonksiyonlar.
│   ├── components/           # Sayfalarda tekrar tekrar kullanılan küçük yapı taşları.
│   │   ├── Navbar.jsx        # Üst menü, çıkış yap butonu.
│   │   ├── TaskItem.jsx      # Tek bir görevin görüntülendiği kart bileşeni.
│   │   └── TaskForm.jsx      # Yeni görev ekleme veya düzenleme formu.
│   ├── pages/                # Tarayıcı url'sine (Route) bağlı olarak render edilen ana ekranlar.
│   │   ├── Login.jsx         # Kullanıcı giriş ekranı.
│   │   ├── Register.jsx      # Yeni kullanıcı kayıt ekranı.
│   │   └── Dashboard.jsx     # Görevlerin listelendiği, filtrelendiği ve yönetildiği ana panel.
│   ├── context/
│   │   └── AuthContext.jsx   # Kullanıcının giriş yapıp yapmadığı bilgisini (Token ve User bilgisi) tüm uygulamaya global olarak dağıtan React Context yapısı.
│   ├── App.jsx               # React Router yönlendirmelerinin (Route kuralları ve Protected Routes) tanımlandığı kök bileşen.
│   └── index.js              # Uygulamayı DOM'a bağlayan başlangıç dosyası.
├── .env                      # API Base URL'i gibi ortama bağlı (geliştirme/canlı) değişkenlerin tutulduğu dosya.
└── package.json              # Projenin NPM bağımlılıkları (react, axios, react-router-dom vs.).
```

---

### Doğrusal Geliştirme İşleyişi (Adım Adım Yol Haritası)

1.  **Altyapı ve Veritabanı Kurulumu (Backend)**
    * .NET Web API projesi oluşturulur.
    * Entity Framework paketleri projeye dahil edilir.
    * `Models` klasöründe `User` ve `TaskItem` sınıfları oluşturulur. `TaskItem` içerisine `UserId` eklenerek yabancı anahtar (Foreign Key) ilişkisi kurulur.
    * `AppDbContext` yapılandırılır, `appsettings.json` içerisine MSSQL connection string yazılır.
    * Migration oluşturulup (`Add-Migration Initial`) veritabanına uygulanır (`Update-Database`).

2.  **Kimlik Doğrulama ve Güvenlik (Backend)**
    * `AuthService` geliştirilir. Kullanıcı kayıt olurken şifresi hashlenerek veritabanına kaydedilir.
    * JWT algoritması entegre edilir. Başarılı girişte Token üretecek metot yazılır.
    * `Program.cs` içerisine JWT Authentication middleware'i eklenir (API'nin token kabul etmesi sağlanır).
    * `AuthController` yazılır ve Postman/Swagger üzerinden yetkilendirme akışı test edilir.

3.  **İş Mantığı ve CRUD Operasyonları (Backend)**
    * `TaskService` ve DTO nesneleri oluşturulur.
    * `TaskController` oluşturulur. Sınıfın başına `[Authorize]` özniteliği (attribute) eklenerek uç noktalar koruma altına alınır.
    * Controller içindeki metotlarda, HTTP isteğini yapan kullanıcının kimliği (`User.Identity`) JWT'den okunur. Görev eklenirken bu kimlik göreve atanır; okuma, güncelleme ve silme işlemlerinde görevin sadece bu kimliğe ait olup olmadığı denetlenir.
    * Tüm CRUD operasyonları Postman üzerinden "Bearer Token" eklenerek test edilir. Cors ayarları yapılandırılarak React'in API'ye erişimine izin verilir.

4.  **Arayüz İskeleti ve Route Kurulumu (Frontend)**
    * React projesi başlatılır (`npm create vite@latest` veya `npx create-react-app`).
    * Gerekli paketler (`axios`, `react-router-dom`) kurulur.
    * `Pages` (Login, Register, Dashboard) oluşturulur. `App.jsx` içerisinde sayfa geçiş (Routing) mimarisi kurulur.

5.  **Global Durum Yönetimi ve API Entegrasyonu (Frontend)**
    * `AuthContext.jsx` oluşturulur. Token'ın localStorage'a yazılması, okunması ve bellekten silinmesi (Logout) işlevleri buraya bağlanır.
    * Yalnızca giriş yapmış kullanıcıların görebileceği "Protected Route" bileşeni oluşturulur ve Dashboard sayfası bu rota ile sarılır.
    * `axiosConfig.js` oluşturulup bir "Interceptor" yazılır. Bu sayede her Axios isteğinin Header'ına localStorage'daki JWT otomatik olarak eklenir.

6.  **UI Bileşenlerinin Kodlanması ve Bağlanması (Frontend)**
    * Auth formları (Login/Register) bağlanır.
    * Dashboard sayfasında durumlar (tasks, statusFilter, categoryFilter) `useState` ile tanımlanır.
    * Sayfa yüklendiğinde (`useEffect` ile) API'den görevler çekilir ve ekrana basılır.
    * Görevin tamamlandı olarak işaretlenmesi (Update) veya silinmesi (Delete) eylemleri, oluşturulan görev kartlarındaki butonlara bağlanarak ilgili API istekleri tetikletilir. İşlem sonrası lokal State güncellenerek sayfanın yenilenmesine gerek kalmadan UI değiştirilir.

7.  **Dağıtım (Deployment)**
    * Backend, `Release` modunda derlenir ve IIS sunucusuna, Azure App Service'e veya AWS EC2 gibi bir platforma yüklenir. MSSQL veritabanı uzak sunucuya taşınır ve Connection String güncellenir.
    * Frontend, `.env` dosyasındaki API URL canlı sunucu adresine göre güncellendikten sonra build (`npm run build`) edilir. Ortaya çıkan statik dosyalar Vercel, Netlify veya AWS S3/Cloudfront gibi servisler üzerinden yayına alınır.