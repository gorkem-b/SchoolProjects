Haklısın. Boş bir şablon vermek büyük bir hataydı, sinirlenmekte sonuna kadar haklısın. Sabahtan beri milimetresine, hacmine ve mimarisine kadar kurguladığımız **Quake tarzı, kapalı üs baskını (Raid)** konseptli projeyi tek bir potada eritiyorum. 

Aşağıda, hocanın önüne koyduğunda "bu adam ne yaptığını, uzamsal geometriyi ve bölüm tasarımını (Level Design) çok iyi biliyor" dedirtecek, **PURGE projesinin nihai ve tam teşekküllü Oyun Tasarım Dokümanı (GDD)** yer alıyor. 

Hiçbir eksiği, yuvarlak lafı yok. Tamamen bizim projemiz.

---

# OYUN TASARIM DOKÜMANI (GDD)
**Proje Adı:** PURGE
**Oyun Motoru:** Unity 3D (FPS Microgame Template)
**Tür:** Birinci Şahıs Nişancı (FPS) / Quake-Tarzı Kapalı Üs Baskını (Tactical Base Raid)
**Tasarım Paradigması:** Güdümlü Lineer (Guided Linear) & Hacimsel Geometrik Bloklama (Volumetric Greyboxing)
**Sürüm:** 1.0 (Nihai Geometrik Sürüm)

## 1. VİZYON VE KONSEPT (Core Vision)
"Purge", oyuncuyu dış dünyaya tamamen mühürlenmiş, yeraltına gömülü devasa bir endüstriyel üsse (Sektör 4) düzenlenen tek kişilik bir sızma ve temizlik (Raid & Clear) operasyonunun merkezine koyar. Oyun; Quake ve Doom gibi klasiklerin "devasa hacimli odalar, agresif ilerleyiş ve acımasız geometri" felsefesini benimser. 

Açık dünya veya hayatta kalma (survival) unsurları yoktur. Amaç basittir: Mühürlü bir üsse zorla gir, devasa betonarme koridorlarda ve ölümcül silolarda aşağı doğru in, önüne çıkan tüm otomatik savunma sistemlerini (HoverBot ve Turret) yok et ve ana reaktörü kapat.

## 2. TEKNİK KAPSAM VE MÜHENDİSLİK (Technical Scope)
Bu proje, Minimum Uygulanabilir Ürün (MVP) ilkesiyle, harici hiçbir 3D model kullanılmadan geliştirilmiştir. 
* **Mimari Motor:** Unity'nin yerleşik `Cube`, `Cylinder` ve `Plane` primitifleri.
* **Metrik Sistem:** 1 Unity Birimi = 1 Metre.
* **Tasarım Felsefesi:** Estetik yerine "Matematiksel Ölçek (Scale)" ve "Görüş Hattı (Line of Sight)" manipülasyonu. Düşman zorluğu kodla değil, devasa geometrik engellerle ve dikey/yatay uzay kullanımıyla sağlanır.

## 3. BÖLÜM MİMARİSİ VE KESİN GEOMETRİK YÖNERGELER (Architectural Blueprint)
Aşağıdaki ölçüler, Unity içerisindeki Transform -> Scale (X, Y, Z) değerlerini milimetrik olarak temsil eder.

### LEVEL 1: LOJİSTİK HANGARI (Üsse Sızma ve Siper Savaşı)
**Tasarım Hedefi:** Quake tarzı devasa bir depo. Kapalı üssün dış savunma hattını yarmak.
* **Dış Duvarlar:** Oyuncuyu içeride tutan, üstü kapalı devasa dikdörtgen prizma. **Scale: X=200, Y=50, Z=400**.
* **Siper Kasaları:** Oyuncunun üzerinden atlayamayacağı büyüklükte 20 adet özdeş kare prizma. **Scale: X=8, Y=8, Z=8**. Girişten çıkışa doğrudan görüşü kesecek şekilde zikzaklı dizilmiştir.
* **Hangarlar:** Harita sonuna (Z=300) konumlandırılmış 2 adet devasa yapı. **Scale: X=60, Y=30, Z=80**.
* **Düşman Dağılımı:** Kasaların tepelerinde yükseklik avantajına sahip HoverBot'lar.

### LEVEL 2: ZIRHLI KORİDORLAR (Kavşak Temizliği)
**Tasarım Hedefi:** L-Dönüşler ve T-Kavşaklardan oluşan, sürekli ilerlemeye (Run & Gun) dayalı dar alan çatışması.
* **Koridor Hacmi:** Tavan yüksekliği (Y) tam 10 metredir. Oyuncunun manevra yapabileceği koridor genişliği (X ve Z boşlukları) tam 15 metredir.
* **Duvar Blokları:** Koridorları oluşturan devasa beton bloklar. **Scale: X=30, Y=10, Z=50**. Bu bloklar yan yana dizilerek kavşaklar oluşturur.
* **Düşman Dağılımı:** Her 90 derecelik L dönüşünün kör noktasına yerleştirilmiş (tavana ters asılı) Turret'lar.

### LEVEL 3: DERİN SİLO İNİŞİ (Dairesel Çatışma)
**Tasarım Hedefi:** Zemini olmayan devasa bir kuyuda sarmal bir rampa üzerinden aşağı inmek.
* **Merkez Sütun:** Kuyunun tam ortasından aşağı inen devasa silindir. **Scale: X=60, Y=150, Z=60**.
* **Dış Duvar:** İçi boş devasa silindir. **Scale: X=140, Y=150, Z=140**.
* **Sarmal Rampa:** Dış duvar ile merkez sütun arasındaki boşlukta, aşağı doğru kıvrılan, **X=40, Y=1, Z=30** boyutlarında düzlemler.
* **Düşman Dağılımı:** Rampa boyunca aşağı inen oyuncuya yukarıdan ve karşıdan ateş eden HoverBot ve Turret çapraz ateşi.

### LEVEL 4: SOĞUTMA SÜTUNLARI ORMANI
**Tasarım Hedefi:** Devasa silindirlerin görüşü %100 kapattığı, saklambaç tarzı agresif bir CQC (Yakın Mesafe) arenası.
* **Arena Kapsamı:** X=200, Z=200 boyutlarında kare bir kapalı oda. Y=30 yüksekliğinde tavan.
* **Sütunlar (Engeller):** Tavana kadar uzanan 40 adet devasa silindir. **Scale: X=20, Y=30, Z=20**.
* **Dizilim Geometrisi:** Sütunların merkezleri arasında 30 metre boşluk vardır. Bu, oyuncuya geçmesi ve savaşması için 10 metrelik dar çatışma yolları yaratır.

### LEVEL 5: REAKTÖR ÇEKİRDEĞİ (Epik Boss Arenası)
**Tasarım Hedefi:** Quake final bölümleri standartlarında, boşlukta asılı, devasa bir dairesel arenada Boss savaşı.
* **Zemin:** Yok. Y=-50 koordinatı mutlak ölüm sınırıdır (Kill Zone).
* **Ana Arena:** Uçurumun ortasında asılı devasa dairesel platform. **Scale: X=160, Y=5, Z=160**.
* **Siper Blokları:** Çemberin dış kenarlarına 45 derecelik açılarla dizilmiş 8 adet beton blok. **Scale: X=10, Y=10, Z=5**.
* **Boss (Ana Çekirdek):** Arenanın tam merkezine yerleştirilmiş, Unity Inspector üzerinden Scale değerleri **X=15, Y=15, Z=15** yapılarak devasa boyuta getirilmiş bir Turret. Çevresinde sürekli düşman üreten Spawner'lar bulunur.

## 4. ARAYÜZ VE HİKAYE AKIŞI (UI & Narrative)
* Olay örgüsü oyuncuyu durduran ara sahnelerle değil, her bölüm başında ekranda 3 saniye belirip kaybolan pasif log metinleriyle (TextMeshPro + Canvas Group Alpha) verilir.
* **L1:** "Üs Mühürlendi. Sızma operasyonu başlıyor."
* **L2:** "Dış savunma aşıldı. Zırhlı geçitlerde ateş serbest."
* **L3:** "Merkez kuyu. Karantina protokolü devrede."
* **L4:** "Soğutma sistemlerine girildi. Hedefleri yok et."
* **L5:** "Ana çekirdek. Baskını tamamla."

