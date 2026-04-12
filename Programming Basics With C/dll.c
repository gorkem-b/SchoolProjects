#include <stdio.h>
#include <stdlib.h> // malloc() ve free() için gerekli

// Düğüm yapısını 'struct NODE' olarak tanımlıyoruz
struct NODE {
    struct NODE *pNext; // 'struct NODE' türünde bir sonrakine işaretçi
    struct NODE *pPrev; // 'struct NODE' türünde bir öncekine işaretçi
    int nData;
};

// Global işaretçileri tanımlarken de 'struct' kullanmalıyız
struct NODE *pHead, *pTail;

// Fonksiyon prototipleri
void AppendNode(struct NODE *pNode);
void InsertNode(struct NODE *pNode, struct NODE *pAfter);
void RemoveNode(struct NODE *pNode);
void DeleteAllNodes(); // Düzeltilmiş versiyonu aşağıda

/**
 * @brief Bir düğümü listenin sonuna ekler.
 */
void AppendNode(struct NODE *pNode)
{
    if (pHead == NULL) { // Liste boşsa
       pHead = pNode;
       pNode->pPrev = NULL;
    }
    else { // Listede eleman varsa
       pTail->pNext = pNode; // Eski son, yeniyi göstersin
       pNode->pPrev = pTail; // Yeni, eski sonu göstersin
    }
    pTail = pNode; // Yeni düğüm artık yeni son (pTail)
    pNode->pNext = NULL;
}

/**
 * @brief pNode'u pAfter düğümünden hemen sonraya ekler.
 */
void InsertNode(struct NODE *pNode, struct NODE *pAfter)
{
    pNode->pNext = pAfter->pNext;
    pNode->pPrev = pAfter;
    
    if (pAfter->pNext != NULL) // Eğer pAfter'dan sonra bir düğüm varsa
       pAfter->pNext->pPrev = pNode; // O düğümün 'prev'i pNode olmalı
    else
       pTail = pNode; // pAfter son düğümdü, şimdi pNode son düğüm
       
    pAfter->pNext = pNode; // pAfter'ın 'next'i pNode olmalı
}

/**
 * @brief Bir düğümü listeden çıkarır (hafızayı serbest bırakmaz).
 */
void RemoveNode(struct NODE *pNode)
{
    // Önceki düğümün 'next' bağlantısını güncelle
    if (pNode->pPrev == NULL) // Düğüm 'head' ise
       pHead = pNode->pNext;
    else
       pNode->pPrev->pNext = pNode->pNext;

    // Sonraki düğümün 'prev' bağlantısını güncelle
    if (pNode->pNext == NULL) // Düğüm 'tail' ise
       pTail = pNode->pPrev;
    else
       pNode->pNext->pPrev = pNode->pPrev;
}

/**
 * @brief TÜM listeyi siler ve hafızayı boşaltır (DÜZELTİLMİŞ).
 */
void DeleteAllNodes()
{
    struct NODE *pCurrent = pHead; // Baştan başla
    struct NODE *pNodeToFree;    // Silinecek düğümü tutmak için geçici işaretçi

    while (pCurrent != NULL) 
    {
        pNodeToFree = pCurrent; // Şu anki düğümü 'silinecek' olarak işaretle
        pCurrent = pCurrent->pNext; // Döngüde bir sonrakine ilerle
        
        // İşaretçiyi ilerlettikten sonra, 'silinecek' düğümün hafızasını serbest bırak
        free(pNodeToFree); 
    }

    // Liste artık tamamen boş. Global işaretçileri sıfırla.
    pHead = NULL;
    pTail = NULL;
    
    printf("\nTum dugumler silindi ve hafiza bosaltildi.\n");
}

// Standart 'int main()' kullanımı
int main()
{
    // C'de struct kullanırken 'struct' anahtar kelimesi gerekir
    struct NODE *pNode;

    // Global işaretçileri başlat
    pHead = NULL;
    pTail = NULL;

    // Listeye 100 öğe ekle
    printf("100 dugum ekleniyor...\n");
    for (int i = 0; i < 100; i++) {
        // Hafızadan yer ayır
        pNode = (struct NODE *)malloc(sizeof(struct NODE));
        if (pNode == NULL) { // Hafıza hatası kontrolü
            printf("Hafiza ayrilamadi!");
            return 1;
        }
        pNode->nData = i;
        AppendNode(pNode); // Listeye ekle
    }

    // Şimdi listedeki her öğeyi baştan sona (pHead -> pTail) yazdır
    printf("Liste (Bastan Sona):\n");
    for (pNode = pHead; pNode != NULL; pNode = pNode->pNext) {
       printf("%d ", pNode->nData);
    }
    printf("\n");
    
    // (İsteğe bağlı) Listeyi sondan başa (pTail -> pHead) yazdırabiliriz
    printf("Liste (Sondan Basa):\n");
    for (pNode = pTail; pNode != NULL; pNode = pNode->pPrev) {
       printf("%d ", pNode->nData);
    }
    printf("\n");

    // Tüm listeyi sil ve hafızayı boşalt
    DeleteAllNodes();
    
    return 0; // Başarıyla bitti
}