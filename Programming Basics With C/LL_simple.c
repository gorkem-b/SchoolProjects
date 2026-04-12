#include <stdio.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *next;
};

// Listenin başını tutan global işaretçi
struct node *head = NULL;
// 'current' isimli kullanılmayan değişken kaldırıldı.

/**
 * @brief Listeyi baştan sona yazdırır.
 */
void printList() {
    struct node *ptr = head;

    printf("\n[head] =>");
    // Listenin başından başla
    while(ptr != NULL) {        
       printf(" %d =>",ptr->data);
       ptr = ptr->next; // Bir sonrakine git
    }

    printf(" [null]\n");
}

/**
 * @brief Listenin BAŞINA yeni bir eleman ekler.
 */
void insert(int data) {
    // 1. Yeni düğüm için hafızada yer ayır
    struct node *link = (struct node*) malloc(sizeof(struct node));
    if (link == NULL) { // Hafıza ayrılamazsa
        printf("Hafiza hatasi!\n");
        return;
    }
    
    // 2. Veriyi ata
    link->data = data;

    // 3. Yeni düğümün 'next'ini şu anki 'head' yap
    link->next = head;

    // 4. 'head'i yeni düğüm olarak güncelle
    head = link;
}

/**
 * @brief Listenin tamamını siler ve ayrılan hafızayı boşaltır.
 */
void freeList() {
    struct node *currentNode = head; // Baştan başla
    struct node *tempNode;

    printf("\nListe temizleniyor...\n");

    while(currentNode != NULL) {
        tempNode = currentNode; // Silinecek düğümü geçici olarak sakla
        currentNode = currentNode->next; // Bir sonraki düğüme ilerle
        free(tempNode); // Saklanan düğümü serbest bırak
    }

    head = NULL; // Liste artık boş
    printf("Hafiza bosaltildi.\n");
}

int main() {
    // LIFO (Last-In, First-Out) sırasıyla eklenecekler:
    insert(10);
    insert(20);
    insert(30);
    insert(1);
    insert(40);
    insert(56); // 56 en son eklendi, bu yüzden en başta olacak

    // Listeyi yazdır
    // Beklenen Çıktı: [head] => 56 => 40 => 1 => 30 => 20 => 10 => [null]
    printList();

    // <-- İYİLEŞTİRME: Program biterken hafızayı temizle -->
    freeList();
    
    return 0;
}