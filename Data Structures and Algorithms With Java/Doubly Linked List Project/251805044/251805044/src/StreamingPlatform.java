public class StreamingPlatform {
    SeriesNode seriesHead;
    SeriesNode seriesTail;

    public StreamingPlatform() {
        this.seriesHead = null;
        this.seriesTail = null;
    }

    public void addSeries(int seriesId, String title, String category, String ageClass) {
        SeriesNode newSeries = new SeriesNode(seriesId, title, category, ageClass);

        SeriesNode temp = seriesHead;
        while (temp != null) {
            if (temp.seriesId == seriesId) {
                System.out.println("HATA: " + seriesId + " ID'li dizi zaten mevcut. Ekleme yapılmadı.");
                return;
            }
            temp = temp.next;
        }

        if (seriesHead == null) {
            seriesHead = newSeries;
            seriesTail = newSeries;
        }
        else if (newSeries.seriesId < seriesHead.seriesId) {
            newSeries.next = seriesHead;
            seriesHead.prev = newSeries;
            seriesHead = newSeries;
        }
        else {
            SeriesNode current = seriesHead;
            while (current.next != null && current.next.seriesId < newSeries.seriesId) {
                current = current.next;
            }

            if (current.next == null) {
                current.next = newSeries;
                newSeries.prev = current;
                seriesTail = newSeries;
            }
            else {
                newSeries.next = current.next;
                current.next.prev = newSeries;
                current.next = newSeries;
                newSeries.prev = current;
            }
        }
        System.out.println("BAŞARILI: '" + title + "' dizisi eklendi.");
    }

    public SeriesNode findSeries(String title) {
        SeriesNode current = seriesHead;
        while (current != null) {
            if (current.title.equalsIgnoreCase(title)) {
                return current;
            }
            current = current.next;
        }
        return null;
    }

    public SeriesNode findSeriesById(int seriesId) {
        SeriesNode current = seriesHead;
        while (current != null) {
            if (current.seriesId == seriesId) {
                return current;
            }
            current = current.next;
        }
        return null;
    }

    public void removeSeries(String title) {
        SeriesNode seriesToRemove = findSeries(title);

        if (seriesToRemove == null) {
            System.out.println("HATA: '" + title + "' adında bir dizi bulunamadı.");
            return;
        }

        if (seriesToRemove == seriesHead) {
            seriesHead = seriesToRemove.next;
        }
        if (seriesToRemove == seriesTail) {
            seriesTail = seriesToRemove.prev;
        }
        if (seriesToRemove.prev != null) {
            seriesToRemove.prev.next = seriesToRemove.next;
        }
        if (seriesToRemove.next != null) {
            seriesToRemove.next.prev = seriesToRemove.prev;
        }

        System.out.println("BAŞARILI: '" + title + "' dizisi ve tüm bölümleri sistemden kaldırıldı.");
    }

    public void printAllSeries() {
        System.out.println("--- TÜM DİZİLER ---");
        if (seriesHead == null) {
            System.out.println("Sistemde hiç dizi yok.");
            return;
        }
        SeriesNode current = seriesHead;
        while (current != null) {
            current.display();
            current = current.next;
        }
        System.out.println("--- Liste Sonu ---");
    }

    public void addEpisode(int seriesId, int seasonId, int episodeId, String caption, String director) {
        SeriesNode series = findSeriesById(seriesId);
        if (series == null) {
            System.out.println("HATA: '" + seriesId + "' dizisi bulunamadı. Bölüm eklenemedi.");
            return;
        }

        EpisodeNode newEpisode = new EpisodeNode(seasonId, episodeId, caption, director);

        if (series.episodeHead == null) {
            series.episodeHead = newEpisode;
        }
        else {
            series.episodeTail.next = newEpisode;
            newEpisode.prev = series.episodeTail;
        }
        series.episodeTail = newEpisode;
    }

    public EpisodeNode findEpisode(SeriesNode series, int seasonId, int episodeId) {
        if (series == null) return null;

        EpisodeNode current = series.episodeHead;
        while (current != null) {
            if (current.seasonId == seasonId && current.episodeId == episodeId) {
                return current;
            }
            current = current.next;
        }
        return null;
    }

    public void editEpisode(String seriesTitle, int seasonId, int episodeId, String newCaption, String newDirector) {
        SeriesNode series = findSeries(seriesTitle);
        if (series == null) {
            System.out.println("HATA: '" + seriesTitle + "' dizisi bulunamadı.");
            return;
        }

        EpisodeNode episode = findEpisode(series, seasonId, episodeId);
        if (episode == null) {
            System.out.println("HATA: S" + seasonId + "E" + episodeId + " bölümü bulunamadı.");
            return;
        }

        System.out.println("Eski Başlık: " + episode.caption);
        System.out.println("Eski Yönetmen: " + episode.director);
        episode.caption = newCaption;
        episode.director = newDirector;
        System.out.println("BAŞARILI: Bölüm bilgileri güncellendi.");
    }

    public void listEpisodes(String seriesTitle) {
        SeriesNode series = findSeries(seriesTitle);
        if (series == null) {
            System.out.println("HATA: '" + seriesTitle + "' dizisi bulunamadı.");
            return;
        }

        System.out.println("--- '" + seriesTitle + "' DİZİSİNİN BÖLÜMLERİ ---");
        if (series.episodeHead == null) {
            System.out.println("Bu dizinin kayıtlı bölümü yok.");
            return;
        }

        EpisodeNode current = series.episodeHead;
        while (current != null) {
            current.display();
            current = current.next;
        }
        System.out.println("--- Liste Sonu ---");
    }

    public void searchByDirector(String directorName) {
        System.out.println("--- '" + directorName + "' TARAFINDAN YÖNETİLEN BÖLÜMLER ---");
        boolean found = false;
        SeriesNode currentSeries = seriesHead;

        while (currentSeries != null) {
            EpisodeNode currentEpisode = currentSeries.episodeHead;
            while (currentEpisode != null) {
                if (currentEpisode.director.equalsIgnoreCase(directorName)) {
                    System.out.println("Dizi: " + currentSeries.title);
                    currentEpisode.display();
                    found = true;
                }
                currentEpisode = currentEpisode.next;
            }
            currentSeries = currentSeries.next;
        }

        if (!found) {
            System.out.println("Bu yönetmene ait bölüm bulunamadı.");
        }
        System.out.println("--- Arama Sonu ---");
    }

    public void searchByAgeClass(String age) {
        System.out.println("--- '" + age + "' YAŞ SINIFINDAKİ DİZİLER ---");
        boolean found = false;
        SeriesNode current = seriesHead;
        while (current != null) {
            if (current.ageClass.equalsIgnoreCase(age)) {
                current.display();
                found = true;
            }
            current = current.next;
        }
        if (!found) {
            System.out.println("Bu yaş sınıfında dizi bulunamadı.");
        }
        System.out.println("--- Arama Sonu ---");
    }

    public void searchByCategory(String category) {
        System.out.println("--- '" + category + "' KATEGORİSİNDEKİ DİZİLER ---");
        boolean found = false;
        SeriesNode current = seriesHead;
        while (current != null) {
            if (current.category.equalsIgnoreCase(category)) {
                current.display();
                found = true;
            }
            current = current.next;
        }
        if (!found) {
            System.out.println("Bu kategoride dizi bulunamadı.");
        }
        System.out.println("--- Arama Sonu ---");
    }
}

// Bu sınıf tüm kodun kullanım iskeletini oluşturur. Daha sonra bu iskelet üzerinden main çalıştırılır.
// SeriesNode'lardan oluşan çift yönlü bağlı listeyi ve bu listeyi işleyen tüm fonksiyonları (dizi ekleme, silme, arama vb.) içerir.

// addSeries(...) fonksiyonu yeni bir SeriesNode oluşturur.
// Tüm listeyi gezerek girilen seriesId'nin zaten var olup olmadığını kontrol eder.
// Varsa hata verir ve eklemez. Liste boşsa, başa ekler veya seriesId'ye göre doğru sıraya (seriesHead'den küçük mü, araya mı, sona mı) ekler.
// Çift yönlü bağlı listenin next ve prev bağlarını doğru şekilde günceller.

// findSeries(String title) bu fonksiyon listeyi baştan (seriesHead) sona gezer.
// Dizi title (başlığı) kullanıcı girdisiyle (büyük/küçük harf duyarsız) eşleşen SeriesNode'u bulup döndürür.
// Bulamazsa null döner.
// "Dizi Silme", "Bölüm Düzenleme" için kullanılır.

// findSeriesById(int seriesId) findSeries ile aynı mantıkta çalışır.
// Diziyi title yerine seriesId ile arar. Bu, addEpisode fonksiyonu için kullanılır.

// removeSeries(String title) bu fonksiyon findSeries(title) fonksiyonunu kullanarak silinecek diziyi bulur.
// Dizi bulunamazsa hata verir.
// Bulunursa, dizinin prev ve next düğümlerini birbirine bağlayarak düğümü listeden çıkarır.
// Eğer silinen düğüm seriesHead veya seriesTail ise, bu referansları da günceller.
// Bu işlemle birlikte diziye bağlı olan tüm EpisodeNode listesi de (Java'nın Çöp Toplayıcısı sayesinde) bellekten silinir.

// printAllSeries() bu fonksiyon listenin başından (seriesHead) başlayarak bir while döngüsü ile sonuna kadar gider.
// Her SeriesNode için display() fonksiyonunu kullanarak dizinin bilgilerini ekrana basar.

// addEpisode(...) bu fonksiyon findSeriesById(seriesId) ile bölümün ekleneceği diziyi bulur.
// Yeni bir EpisodeNode oluşturur.
// Dizinin episodeHead (bölüm listesi başı) boşsa, bölümü başa ve sona (episodeTail) ekler.
// Liste boş değilse, bölümü episodeTail kullanarak listenin sonuna ekler ve episodeTail referansını günceller.

// findEpisode fonksiyonu belirli bir SeriesNode (dizi) içinde seasonId ve episodeId kullanarak bölüm arar.
// Bölümü bulursa EpisodeNode'u döndürür, bulamazsa null döner.
// editEpisode fonksiyonu için kullanılır.

// editEpisode(...) findSeries ile diziyi, ardından findEpisode ile ilgili bölümü bulur.
// Bölüm bulunursa, newCaption ve newDirector parametreleriyle bölümün caption ve director bilgilerini günceller.

// listEpisodes(String seriesTitle) findSeries ile ilgili diziyi bulur.
// Dizinin episodeHead referansından başlayarak bir while döngüsü ile o diziye ait tüm bölümleri display() metodunu kullanarak listeler.

// searchByDirector(String directorName) İç içe iki while döngüsü kullanır.
// Dış döngü (currentSeries) tüm dizileri (seriesHead'den seriesTail'e) gezer.
// İç döngü (currentEpisode) o anki dizinin tüm bölümlerini (episodeHead'den episodeTail'e) gezer.
// directorName ile eşleşen her bölümün dizi adını ve bölüm bilgilerini yazdırır.

// searchByAgeClass(String age) seriesHead'den başlayarak while döngüsü ile tüm dizileri gezer.
// ageClass alanı age parametresiyle (büyük/küçük harf duyarsız) eşleşen dizileri display() metoduyla yazdırır.

// searchByCategory(String category) seriesHead'den başlayarak while döngüsü ile tüm dizileri gezer.
// category alanı category parametresiyle (büyük/küçük harf duyarsız) eşleşen dizileri display() metoduyla yazdırır.