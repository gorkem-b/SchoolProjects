#include <stdio.h>
#include <stdlib.h>

// Düğüm (node) yapısını tanımlıyoruz.
struct node {
    double x; // Veriyi (sayı) tutar
    struct node *next; // Bir sonraki düğümün adresini tutar
};

// Kodun daha okunaklı olması için typedef (tür tanımlama) kullanıyoruz.
typedef struct node node;
typedef struct node* nodePtr; // nodePtr, bir 'node' yapısının adresini (işaretçisini) temsil eder.
typedef struct node** nodePtrPtr; // nodePtrPtr, bir 'nodePtr' işaretçisinin adresini temsil eder.
                                 // Listenin başını (head) değiştirebilmek için buna ihtiyacımız var.

FILE *inpf; // Dosya işaretçisi

// Fonksiyon prototipleri
int insert(nodePtrPtr, double);
nodePtr delete(nodePtrPtr, double);
nodePtr search(nodePtr, double, int *);

// C standartlarına göre main fonksiyonu 'int' döndürmelidir.
int main(){ // <-- 'void main()' yerine 'int main()'

    nodePtr hdr, p; // hdr: listenin başını (head) tutar. p: geçici bir işaretçi.
    double num;    // Dosyadan veya kullanıcıdan okunan sayı.
    char op;       // Kullanıcının girdiği işlem karakteri (d, s, i, e, l).
    int posn;      // Arama sonucunda bulunan konum (index).

    // Dosyayı okuma ('r') modunda aç.
    if((inpf = fopen("input.txt","r")) == NULL){
        printf("Girdi Dosyasi Acilamadi!\n");
        return 1; // Program hata ile sonlandı.
    }
    
    hdr = NULL; // Liste başlangıçta boş.

    // Dosyadan sayıları oku ve listeye ekle.
    // HATA DÜZELTMESİ: 'while(!feof(inpf))' yerine 'fscanf'in dönüş değerini kontrol et.
    // fscanf, başarıyla okuduğu öğe sayısını döndürür. 1 öğe (%lf) bekliyoruz.
    while (fscanf(inpf,"%lf",&num) == 1) { // <-- Daha güvenli dosya okuma
        insert(&hdr,num); // 'hdr'nin adresini yolluyoruz ki 'insert' onu değiştirebilsin.
    }
    fclose(inpf); // Dosyayı kapat.

    // Listenin ilk durumunu yazdır.
    printf("Dosyadan okunan liste:\n");
    p = hdr;    
    while (p != NULL) {
        printf("%5.4lf ",p->x);
        p=p->next;
    }
    
    printf("\n>>"); // Kullanıcıdan komut bekle.

    // Kullanıcı 'e' (exit) girene kadar döngüye devam et.
    do {
        // HATA DÜZELTMESİ: 'scanf'ten önce bir boşluk ekle.
        // Bu, önceki 'scanf'ten kalan '\n' (Enter) karakterini yok sayar.
        scanf(" %c",&op); // <-- '\n' sorununu düzeltir.

        switch (op) { 
            case 'd': // Delete
                scanf("%lf",&num); 
                p = delete(&hdr,num); // Silmeyi dene
                if(p != NULL) { // Eğer 'delete' bir düğüm adresi döndürdüyse
                    free(p); // Düğümün hafızasını serbest bırak.
                    printf("%5.4lf silindi.\n", num);
                }
                printf(">>");    
                break;
                
            case 's': // Search
                scanf("%lf", &num); 
                p = search(hdr, num, &posn); // Aramayı yap
                
                // HATA DÜZELTMESİ: 'posn > 0' yerine 'posn >= 0'
                // Çünkü ilk elemanın konumu (index) 0'dır.
                if (posn >= 0) { // <-- Düzeltme
                    printf("%5.4lf bulundu. Kayit no: %d\n", num, posn+1); // (posn+1) kullanıcıya 1'den başlayan sıra no verir.
                }
                printf(">>");
                break;
                
            case 'i': // Insert
                scanf("%lf", &num); 
                insert(&hdr,num);
                printf(">>");    
                break;
                
            case 'e': // Exit
                printf("Program sonlandiriliyor...\n");
                break;
            
            case 'l': // List
                p=hdr;    
                if (p == NULL) {
                    printf("Liste bos.\n");
                }
                while(p != NULL){
                    printf("%5.4lf ",p->x);
                    p = p->next;
                }
                printf("\n>>"); 
                break;
        }
    } while (op!='e');

    // Programdan çıkmadan önce tüm listeyi temizleyerek hafızayı boşaltmak iyi bir pratiktir.
    p = hdr;
    while (p != NULL) {
        nodePtr temp = p; // Geçici olarak şu anki düğümü sakla
        p = p->next; // Bir sonrakine git
        free(temp); // Saklanan düğümü serbest bırak
    }

    return 0; // Program başarıyla sonlandı.
}    
    
/**
 * @brief Sıralı bağlı listeye yeni bir eleman ekler.
 * @param header Listenin başını gösteren işaretçinin adresi (nodePtrPtr).
 * @param x Eklenecek double sayı.
 * @return 1 (başarılı) veya 0 (zaten var).
 */
int insert(nodePtrPtr header, double x){

    nodePtr newNode, temp, prev;

    // 1. Yeni düğüm için hafızadan yer ayır ve veriyi ata.
    newNode = malloc(sizeof(node));
    if (newNode == NULL) { // Hafıza ayrılamazsa
        printf("Hafiza hatasi!");
        return 0; // Başarısız
    }
    newNode->x = x;
    newNode->next = NULL;

    // 2. Doğru konumu bulmak için listeyi gez.
    temp = *header; // temp, listenin başından başlar.
    prev = NULL; // prev, başlangıçta NULL (başlangıçta bir "önceki" yok).

    // KRİTİK HATA DÜZELTMESİ:
    // Orijinal kodda, 'if'ten sonraki 'else' bloğu { } parantezlerine sahip değildi.
    // Bu, 'temp=*header;' satırının 'else'e ait olmasına,
    // ancak 'while' döngüsünün her zaman çalışmasına neden oluyordu.
    // Liste boşsa, 'temp' başlatılmamış bir değerle kullanılıyordu (ÇÖKME).

    // 3. Listenin doğru yerini bul.
    // Döngü koşulu:
    // 1. temp != NULL : Listenin sonuna gelmedik.
    // 2. temp->x < newNode->x : Ekleyeceğimiz sayı (newNode->x)
    //    şu anki düğümden (temp->x) büyük olduğu sürece ilerle.
    while (temp != NULL && temp->x < newNode->x) {
        prev = temp; // Şu anki düğümü 'prev' yap
        temp = temp->next; // Bir sonrakine ilerle
    }

    // 4. Aynı eleman kontrolü (Döngü bittikten sonra).
    if (temp != NULL && temp->x == newNode->x) {
        printf("Bu sayi zaten listede var.\n");
        free(newNode); // Oluşturulan ama eklenmeyen düğümü serbest bırak.
        return 0;
    }

    // 5. Düğümü ekle.
    if (prev == NULL) {
        // Döngü hiç çalışmadıysa (ya liste boştu ya da ilk elemandan küçük)
        // Yeni düğümü listenin başına ekle.
        newNode->next = *header; // Yeni düğüm, eski 'header'ı göstersin.
        *header = newNode; // 'header' artık yeni düğümü göstersin.
    }
    else {
        // Düğümü araya veya sona ekle.
        prev->next = newNode; // Önceki düğüm, yeni düğümü göstersin.
        newNode->next = temp; // Yeni düğüm, 'temp'i (sonraki düğümü) göstersin.
    }
    
    return 1;
}    
    
/**
 * @brief Sıralı bağlı listeden bir elemanı siler.
 * @param header Listenin başını gösteren işaretçinin adresi (nodePtrPtr).
 * @param num Silinecek double sayı.
 * @return Silinen düğümün adresi (free() için) veya NULL (bulunamadı).
 */
nodePtr delete(nodePtrPtr header, double num){
    
    nodePtr temp, prev;
    
    // 1. Liste boş mu kontrol et.
    if (*header == NULL) {
        printf("Liste bos!\n"); 
        return NULL;
    }
    
    // 2. Silinecek düğümü ara.
    temp = *header;
    prev = NULL;

    // Aradığımız sayıdan (num) küçük olduğu sürece ilerle.
    while (temp != NULL && temp->x < num) {
        prev = temp;
        temp = temp->next;
    }
        
    // 3. Düğüm bulundu mu kontrol et.
    // Eğer temp NULL ise veya temp->x aranan sayı değilse (daha büyüğünü bulduysak)
    if (temp == NULL || temp->x != num) {
        printf("Silinecek sayi bulunamadi.\n"); 
        return NULL; 
    }
    
    // 4. Düğüm bulundu (temp->x == num). Şimdi sil.
    if (prev == NULL) {
        // Silinecek düğüm listenin başı (ilk eleman).
        *header = temp->next; // Listenin başını bir sonrakine kaydır.
    }
    else {
        // Silinecek düğüm arada veya sonda.
        prev->next = temp->next; // Önceki düğümü, sonrakine bağla (aradan çıkar).
    }
    
    return temp; // Silinen düğümün adresini döndür (free() için).
}

/**
 * @brief Sıralı bağlı listede bir elemanı arar.
 * @param header Listenin başı (sadece okuma, değiştirmeye gerek yok).
 * @param x Aranan double sayı.
 * @param ctr Bulunan elemanın 0'dan başlayan konumunu (index) tutacak değişkenin adresi.
 * @return Bulunan düğümün adresi veya NULL (bulunamadı).
 */
nodePtr search(nodePtr header, double x, int *ctr){ 

    nodePtr temp = header;
    *ctr = 0; // Konum sayacını sıfırla.
    
    // 1. Düğümü ara.
    // Aradığımız sayıdan (x) küçük olduğu sürece ilerle.
    while (temp != NULL && temp->x < x) {
        temp = temp->next; 
        (*ctr)++; // Konumu bir artır.
    }
    
    // 2. Döngü bittiğinde kontrol et.
    if (temp == NULL || temp->x != x) {
        // Ya listenin sonuna geldik ya da aradığımızdan büyük bir sayı bulduk.
        *ctr = -1; // Bulunamadı.
        printf("Aranan sayi listede yok. "); 
        return NULL; 
    } 
    
    // 3. Eleman bulundu (temp->x == x)
    return temp;
}