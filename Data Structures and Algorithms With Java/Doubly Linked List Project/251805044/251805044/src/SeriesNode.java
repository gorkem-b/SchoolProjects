public class SeriesNode {
    int seriesId;
    String title;
    String category;
    String ageClass;

    SeriesNode next;
    SeriesNode prev;

    EpisodeNode episodeHead;
    EpisodeNode episodeTail;

    public SeriesNode(int seriesId, String title, String category, String ageClass) {
        this.seriesId = seriesId;
        this.title = title;
        this.category = category;
        this.ageClass = ageClass;
        this.next = null;
        this.prev = null;
        this.episodeHead = null;
        this.episodeTail = null;
    }

    public void display() {
        System.out.println("\n[ID: " + seriesId + "] " + title);
        System.out.println("  Kategori: " + category + ", Yaş Sınırı: " + ageClass);
    }
}

// Bir diziye ait bilgileri ve o diziye ait bölüm listesinin (EpisodeNode listesi) başını ve sonunu tutar.
// Ana çift yönlü bağlı listenin (diziler listesi) düğüm yapısıdır. Ödevdeki seriesNode yapısına karşılık gelir