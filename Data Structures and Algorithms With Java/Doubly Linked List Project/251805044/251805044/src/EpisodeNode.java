public class EpisodeNode {
    int seasonId;
    int episodeId;
    String caption;
    String director;
    EpisodeNode next;
    EpisodeNode prev;

    public EpisodeNode(int seasonId, int episodeId, String caption, String director) {
        this.seasonId = seasonId;
        this.episodeId = episodeId;
        this.caption = caption;
        this.director = director;
        this.next = null;
        this.prev = null;
    }

    public void display() {
        System.out.println("  > S" + seasonId + "E" + episodeId + ": " + caption + " (Director: " + director + ")");
    }
}

// Tek bir bölüme ait verileri ve o bölümün çift yönlü bağlı listedeki yerini tutar.
// Ödevdeki episodeNode yapısına karşılık gelir.