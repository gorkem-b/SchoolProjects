**Proje İşleyişi**

Sistem, istemcilerin HTTP istekleri üzerinden iletişim kurduğu durumsuz (stateless) bir RESTful web servisidir. İstemci, ilk olarak kayıt ve giriş uç noktalarına istek atarak bir JWT (JSON Web Token) elde eder. Sistem, istemcinin oturum bilgisini kendi üzerinde tutmaz; tüm kimlik doğrulama verisi bu token içerisine şifrelenir.

Başarılı bir girişten sonra istemci, görevler (tasks) üzerinde işlem yapmak için her HTTP isteğinin başlığına (header) bu JWT'yi ekler. API'ye gelen her görev isteği, öncelikle kimlik doğrulama katmanından geçer. Token geçerliyse, istekteki kimlik bilgisi (user ID) çözümlenir ve sistem veritabanında sadece bu ID'ye ait olan görevler üzerinde okuma, yazma, güncelleme veya silme (CRUD) işlemi yapar. Tüm sistem, dış ortamdan izole edilmiş bir Docker konteyneri içerisinde kendi veritabanı dosyasıyla birlikte çalışır.

**Teknoloji Yığını (Tech-Stack)**

* **Çalışma Ortamı:** Docker
* **Programlama Dili:** Python 3.x
* **Web Framework:** Flask
* **Veritabanı:** SQLite (Dosya tabanlı, yerleşik)
* **ORM (Veritabanı Yönetimi):** Flask-SQLAlchemy (Veritabanı sorgularını soyutlamak için)
* **Kimlik Doğrulama:** PyJWT (Token üretimi ve doğrulaması)
* **Şifreleme:** Werkzeug.security (Şifreleri hashlemek için Flask dahili aracı)

**Sistem Tasarımı (System Design)**

Sistem, İstemci-Sunucu (Client-Server) modeline dayalı monolitik bir tasarıma sahiptir. Dışarıdan gelen HTTP istekleri sunucuya ulaşır, sunucu iş mantığını çalıştırır, veritabanına erişir ve sadece JSON formatında veri döner (HTML render edilmez). Veritabanı hafif olduğu için ayrı bir sunucuda değil, uygulamanın dosya sisteminde barınır. Ölçeklenebilirlik gereksinimi olmayan bu düzey bir proje için donanım kaynaklarını minimumda tutan, doğrudan ve senkron bir iletişim modeli benimsenmiştir.



**Mimarinin İnşası (Architecture Construction)**

Mimari, Application Factory (Uygulama Fabrikası) ve Blueprints (Modüler Tasarım) desenleri üzerine inşa edilir. Bu, kodun tek bir dosyada yığılmasını engeller ve mantıksal birimlere ayrılmasını sağlar.
Mimari 3 ana katmandan oluşur:
1.  **Veri Katmanı (Models):** Veritabanı şemalarının Python sınıfları olarak tanımlandığı yerdir (Kullanıcı ve Görev tabloları).
2.  **Yönlendirme ve İş Mantığı Katmanı (Controllers/Routes):** Gelen HTTP isteklerini karşılar, veriyi doğrular, iş kurallarını uygular ve ilgili veri katmanına iletir. İki ana modüle ayrılır: Kimlik Doğrulama (Auth) ve Görevler (Tasks).
3.  **Güvenlik Katmanı (Middlewares/Decorators):** İstekler yönlendirme katmanına düşmeden önce token doğrulamasını yapan ara katmandır.



**Kod Akış Diyagramı (Flow Diagram)**

**A. Kimlik Doğrulama (Login) Akışı:**
1. İstemci -> `POST /login` (JSON: email, şifre)
2. Uygulama -> Veritabanından e-posta adresini sorgula.
3. Uygulama -> Gelen şifre ile veritabanındaki hash'lenmiş şifreyi karşılaştır.
4. Eşleşme Başarılı -> Payload'ında `user_id` bulunan bir JWT oluştur.
5. Uygulama -> İstemci (HTTP 200, JSON: Token)

**B. Korumalı Kaynağa Erişim (Görev Ekleme) Akışı:**
1. İstemci -> `POST /tasks` (Header: `Authorization: Bearer <token>`, JSON: Görev Detayları)
2. Ara Katman (Decorator) -> Token mevcut mu? (Hayır -> HTTP 401 Unauthorized)
3. Ara Katman -> Token geçerli mi/süresi dolmuş mu? (Hayır -> HTTP 401 Unauthorized)
4. Ara Katman -> Token içinden `user_id` değerini çıkar. İsteği ilgili uç noktaya (Route) aktar.
5. Uç Nokta -> Görev verisini ve çıkarılan `user_id` bilgisini alarak veritabanında yeni satır oluştur.
6. Uygulama -> İstemci (HTTP 201 Created, JSON: Eklenen Görev Detayı)



**Klasör Yapısı ve Sorumluluklar**

```text
task_manager_api/
├── Dockerfile              # Projeyi container haline getirecek izolasyon talimatları.
├── .dockerignore           # Container'a kopyalanmayacak dosyalar (örn. venv, __pycache__).
├── requirements.txt        # Pip paket bağımlılık listesi. Container ayağa kalkarken kurulur.
├── run.py                  # Uygulamayı ayağa kaldıran tek satırlık ana tetikleyici dosya.
├── instance/               
│   └── app.db              # SQLite veritabanı dosyası. Çalışma anında otomatik oluşur.
└── app/                    
    ├── __init__.py         # Flask app nesnesini, DB'yi başlatan ve Blueprint'leri bağlayan dosya.
    ├── config.py           # Gizli anahtarlar, veritabanı yolu gibi çevresel değişken tanımları.
    ├── models.py           # SQLAlchemy User ve Task sınıfları (DB tabloları ve birbirleriyle ilişkileri).
    ├── auth/               
    │   ├── __init__.py     # Auth modülünü Blueprint olarak tanımlayan dosya.
    │   └── routes.py       # Kullanıcı kaydı ve giriş işlemleri (/register, /login).
    ├── tasks/              
    │   ├── __init__.py     # Tasks modülünü Blueprint olarak tanımlayan dosya.
    │   └── routes.py       # Görev ekleme, silme, okuma, güncelleme uç noktaları.
    └── utils/              
        └── decorators.py   # Token doğrulamasını yapan, uç noktaları koruyan sarmalayıcı (wrapper) fonksiyon.
```

**Doğrusal Geliştirme İşleyişi (Adım Adım)**

1.  **Çevre Kurulumu:** Yerel klasör oluşturulur, `python -m venv venv` ile sanal ortam başlatılır. Flask, Flask-SQLAlchemy, PyJWT paketleri kurulur ve `pip freeze > requirements.txt` ile kayıt altına alınır.
2.  **Konfigürasyon ve Temel Çatı:** `config.py` içinde `SECRET_KEY` ve `SQLALCHEMY_DATABASE_URI` (sqlite:///app.db) belirlenir. `app/__init__.py` içinde Flask uygulaması tanımlanır ve veritabanı nesnesine bağlanır.
3.  **Veritabanı Modellenmesi:** `models.py` yazılır. `User` tablosunda ID, e-posta, hashli şifre sütunları; `Task` tablosunda ID, başlık, açıklama, kategori, durum, `user_id` (foreign key) sütunları oluşturulur.
4.  **Güvenlik Aracının Geliştirilmesi:** `utils/decorators.py` içine istek başlığındaki `Bearer` token'ı alıp çözen `token_required` fonksiyonu yazılır. Token yoksa veya hatalıysa isteğin işlem görmeden reddedilmesi sağlanır.
5.  **Kimlik Doğrulama Uç Noktaları:** `auth/routes.py` kodlanır. `POST /register` ile gelen kullanıcının şifresi `werkzeug` ile hashlenip DB'ye kaydedilir. `POST /login` ile doğrulama yapılır ve PyJWT ile `user_id` taşıyan, belirli bir süre geçerli token üretilir.
6.  **Görev Yönetimi Uç Noktaları:** `tasks/routes.py` kodlanır. Tüm uç noktalar `token_required` ile korunur.
    * `POST /tasks`: Yeni görev ekler. Görev otomatik olarak giriş yapan `user_id` ile eşleştirilir.
    * `GET /tasks`: URL parametrelerine göre filtreleme (durum, kategori) yapılarak, sadece istekte bulunan kullanıcının görevleri listelenir.
    * `GET /tasks/<id>`: Spesifik bir görevi getirir. Görevin `user_id`'si ile token'daki ID uyuşmuyorsa yetki reddedilir.
    * `PUT /tasks/<id>` ve `DELETE /tasks/<id>`: Görev güncelleme ve silme işlemleri `user_id` doğrulaması yapılarak gerçekleştirilir.
7.  **Sistemin Bütünleştirilmesi:** `auth` ve `tasks` dizinlerindeki Blueprint nesneleri `app/__init__.py` içine import edilip uygulamaya (`app.register_blueprint`) kaydedilir.
8.  **Dockerization (Konteynerleştirme):** `Dockerfile` yazılır. İşletim sistemi olarak hafif bir Python imajı (`python:3.10-slim`) seçilir. Kaynak kodlar içeri kopyalanır, `pip install -r requirements.txt` komutu Dockerfile içinde verilir. Container başladığında `python run.py` komutunun çalıştırılması sağlanır.
9.  **Dağıtım (Deploy Hazırlığı):** `docker build -t taskmanager .` ile imaj oluşturulur, `docker run -p 5000:5000 taskmanager` komutuyla sistem yerel bir sunucu gibi ayağa kaldırılır. Veritabanı container içinde otomatik oluşur. API testleri Postman üzerinden yapılarak süreç tamamlanır.