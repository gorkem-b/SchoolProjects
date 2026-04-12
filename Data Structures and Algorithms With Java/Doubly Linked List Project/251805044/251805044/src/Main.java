static StreamingPlatform platform = new StreamingPlatform();
static Scanner scanner = new Scanner(System.in);

void main() {
    loadFiles();
    runMenu();
}

public static void loadFiles() {
    try (Scanner fileScanner = new Scanner(new File("src/series.txt"))) {
        while (fileScanner.hasNextLine()) {
            String line = fileScanner.nextLine();
            String[] parts = line.split(",");
            if (parts.length == 4) {
                int id = Integer.parseInt(parts[0]);
                String title = parts[1];
                String category = parts[2];
                String ageClass = parts[3];
                platform.addSeries(id, title, category, ageClass);
            }
        }
        IO.println("\n'series.txt' dosyası başarıyla yüklendi.");
    } catch (FileNotFoundException e) {
        IO.println("HATA: 'series.txt' dosyası bulunamadı.");
    } catch (NumberFormatException e) {
        IO.println("HATA: 'series.txt' dosyasında format hatası (ID sayı olmalı).");
    }

    try (Scanner fileScanner = new Scanner(new File("src/episodes.txt"))) {
        while (fileScanner.hasNextLine()) {
            String line = fileScanner.nextLine();
            String[] parts = line.split(",");
            if (parts.length == 5) {
                int id = Integer.parseInt(parts[0]);
                int seasonId = Integer.parseInt(parts[1]);
                int episodeId = Integer.parseInt(parts[2]);
                String caption = parts[3];
                String director = parts[4];
                platform.addEpisode(id, seasonId, episodeId, caption, director);
            }
        }
        IO.println("'episodes.txt' dosyası başarıyla yüklendi.");
    } catch (FileNotFoundException e) {
        IO.println("HATA: 'episodes.txt' dosyası bulunamadı.");
    } catch (NumberFormatException e) {
        IO.println("HATA: 'episodes.txt' dosyasında format hatası (ID'ler sayı olmalı).");
    }
}

public static void runMenu() {
    int choice = -1;
    while (choice != 0) {
        IO.println("\n--- STREAMING PLATFORMU YÖNETİM SİSTEMİ ---");
        IO.println("1. Yeni Dizi Ekle");
        IO.println("2. Dizi Sil");
        IO.println("3. Tüm Dizileri Listele");
        IO.println("4. Yeni Bölüm Ekle");
        IO.println("5. Bölüm Düzenle");
        IO.println("6. Dizinin Bölümlerini Listele");
        IO.println("7. Yönetmene Göre Ara");
        IO.println("8. Yaş Sınıfına Göre Ara");
        IO.println("9. Kategoriye Göre Ara");
        IO.println("0. Çıkış");
        IO.print("Seçiminiz: ");

        try {
            choice = scanner.nextInt();
            scanner.nextLine();

            switch (choice) {
                case 1:
                    opAddSeries();
                    break;
                case 2:
                    opRemoveSeries();
                    break;
                case 3:
                    platform.printAllSeries();
                    break;
                case 4:
                    opAddEpisode();
                    break;
                case 5:
                    opEditEpisode();
                    break;
                case 6:
                    opListEpisodes();
                    break;
                case 7:
                    opSearchDirector();
                    break;
                case 8:
                    opSearchAgeClass();
                    break;
                case 9:
                    opSearchCategory();
                    break;
                case 0:
                    IO.println("Programdan çıkılıyor...");
                    break;
                default:
                    IO.println("Geçersiz seçim. Lütfen 0-9 arası bir sayı girin.");
            }
        } catch (Exception e) {
            IO.println("HATA: Geçersiz giriş. Lütfen bir sayı girin.");
            scanner.nextLine();
            choice = -1;
        }
    }
    scanner.close();
}

private static void opAddSeries() {
    try {
        IO.print("Series ID: ");
        int id = scanner.nextInt();
        scanner.nextLine();
        IO.print("Title: ");
        String title = scanner.nextLine();
        IO.print("Category: ");
        String category = scanner.nextLine();
        IO.print("Age Class (örn: 13+): ");
        String age = scanner.nextLine();
        platform.addSeries(id, title, category, age);
    } catch (Exception e) {
        IO.println("HATA: Geçersiz ID girdiniz.");
        scanner.nextLine();
    }
}

private static void opRemoveSeries() {
    IO.print("Silinecek dizinin adı (Title): ");
    String title = scanner.nextLine();
    platform.removeSeries(title);
}

private static void opAddEpisode() {
    try {
        IO.print("Bölüm eklenecek dizinin adı (Title): ");
        int title = Integer.parseInt(scanner.nextLine());
        IO.print("Season ID: ");
        int sId = scanner.nextInt();
        IO.print("Episode ID: ");
        int eId = scanner.nextInt();
        scanner.nextLine();
        IO.print("Caption (Bölüm Başlığı): ");
        String caption = scanner.nextLine();
        IO.print("Director (Yönetmen): ");
        String director = scanner.nextLine();
        platform.addEpisode(title, sId, eId, caption, director);
    } catch (Exception e) {
        IO.println("HATA: Geçersiz ID girdiniz.");
        scanner.nextLine();
    }
}

private static void opEditEpisode() {
    try {
        IO.print("Düzenlenecek bölümün dizisinin adı (Title): ");
        String title = scanner.nextLine();
        IO.print("Düzenlenecek bölümün Season ID'si: ");
        int sId = scanner.nextInt();
        IO.print("Düzenlenecek bölümün Episode ID'si: ");
        int eId = scanner.nextInt();
        scanner.nextLine();

        SeriesNode series = platform.findSeries(title);
        EpisodeNode episode = platform.findEpisode(series, sId, eId);

        if (episode == null) {
            IO.println("HATA: Bölüm bulunamadı.");
            return;
        }
        IO.println("Bölüm bulundu: " + episode.caption + " (Yönetmen: " + episode.director + ")");
        IO.print("Yeni Caption (Başlık): ");
        String newCaption = scanner.nextLine();
        IO.print("Yeni Director (Yönetmen): ");
        String newDirector = scanner.nextLine();
        platform.editEpisode(title, sId, eId, newCaption, newDirector);
    } catch (Exception e) {
        IO.println("HATA: Geçersiz ID girdiniz.");
        scanner.nextLine();
    }
}

private static void opListEpisodes() {
    IO.print("Bölümleri listelenecek dizinin adı (Title): ");
    String title = scanner.nextLine();
    platform.listEpisodes(title);
}

private static void opSearchDirector() {
    IO.print("Aranacak yönetmenin adı: ");
    String director = scanner.nextLine();
    platform.searchByDirector(director);
}

private static void opSearchAgeClass() {
    IO.print("Aranacak yaş sınıfı (örn: 16+): ");
    String age = scanner.nextLine();
    platform.searchByAgeClass(age);
}

private static void opSearchCategory() {
    IO.print("Aranacak kategori (örn: Drama): ");
    String category = scanner.nextLine();
    platform.searchByCategory(category);
}

// Projenin motoru, StreamingPlatform sınıfını kullanarak programı başlatır.
// Başlangıç verilerini (.txt dosyaları) yükler  ve kullanıcıya bir ana menü sunar.

// Önce loadFiles() fonksiyonu  series.txt ve episodes.txt dosyalarındaki verileri bağlı listelere yükler.

// runMenu() fonksiyonu ana menü döngüsünü başlatır.

// loadFiles() series.txt ve episodes.txt dosyalarını okur.
// Her satırı okur, virgül (,) karakterine göre böler (line.split(",")) ve verileri (ID, title, category vb.) ayırır.

// Her dizi satırı için platform.addSeries() metodunu çağırarak diziyi sıralı bir şekilde listeye ekler.

// Her bölüm satırı için platform.addEpisode() metodunu çağırarak bölümü ilgili dizinin listesinin sonuna ekler.

// runMenu() bu fonksiyon programın ana menüsünü yönetir.
// Kullanıcı 0 (Çıkış) seçeneğini girene kadar çalışan bir while döngüsü içerir.
// Menünün her işlemden sonra tekrar görünmesini sağlar.

// opAddSeries() Kullanıcıdan ID, başlık, kategori ve yaş sınıfı alır ve platform.addSeries()'i çağırır.

// opRemoveSeries() silinecek dizinin adını alır ve platform.removeSeries()'i çağırır.

// opAddEpisode() dizi ID'si, sezon/bölüm ID'si, başlık ve yönetmen alır ve platform.addEpisode()'u çağırır.

// opEditEpisode() dizi adı, sezon/bölüm ID'si alır, eski bölümü bulur, yeni bilgileri alır ve platform.editEpisode()'u çağırır.

// opListEpisodes() dizi adını alır ve platform.listEpisodes()'u çağırır.

// opSearchDirector() yönetmen adını alır ve platform.searchByDirector()'ı çağırır.

// opSearchAgeClass() yaş sınıfını alır ve platform.searchByAgeClass()'ı çağırır.

// opSearchCategory() kategori adını alır ve platform.searchByCategory()'ı çağırır.