using Unity.FPS.Game;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.FPS.UI
{
    public class CompassElement : MonoBehaviour
    {
        [Tooltip("The marker on the compass for this element")]
        public CompassMarker CompassMarkerPrefab;

        [Tooltip("Text override for the marker, if it's a direction")]
        public string TextDirection;

        Compass m_Compass;

        void Awake()
        {
            m_Compass = FindFirstObjectByType<Compass>();
            if (m_Compass == null)
            {
                enabled = false;
                return;
            }

            CompassMarker markerInstance = CompassMarkerPrefab != null
                ? Instantiate(CompassMarkerPrefab)
                : CreateFallbackMarker();

            if (markerInstance == null)
            {
                enabled = false;
                return;
            }

            markerInstance.Initialize(this, TextDirection);
            m_Compass.RegisterCompassElement(transform, markerInstance);
        }

        void OnDestroy()
        {
            if (m_Compass != null)
                m_Compass.UnregisterCompassElement(transform);
        }

        CompassMarker CreateFallbackMarker()
        {
            GameObject markerRoot = new GameObject(name + "_CompassMarker");
            RectTransform rectTransform = markerRoot.AddComponent<RectTransform>();
            rectTransform.sizeDelta = new Vector2(18f, 18f);

            CanvasGroup canvasGroup = markerRoot.AddComponent<CanvasGroup>();
            canvasGroup.alpha = 0f;

            CompassMarker marker = markerRoot.AddComponent<CompassMarker>();
            marker.CanvasGroup = canvasGroup;
            marker.DefaultColor = Color.white;
            marker.AltColor = Color.red;

            GameObject imageObject = new GameObject("Image");
            imageObject.transform.SetParent(markerRoot.transform, false);
            RectTransform imageRect = imageObject.AddComponent<RectTransform>();
            imageRect.anchorMin = Vector2.zero;
            imageRect.anchorMax = Vector2.one;
            imageRect.offsetMin = Vector2.zero;
            imageRect.offsetMax = Vector2.zero;

            Image image = imageObject.AddComponent<Image>();
            image.sprite = Resources.GetBuiltinResource<Sprite>("UI/Skin/UISprite.psd");
            image.raycastTarget = false;
            marker.MainImage = image;

            Objective objective = GetComponent<Objective>() ?? GetComponentInParent<Objective>();
            if (objective != null)
            {
                marker.DefaultColor = objective.IsBlocking() ? new Color(0.3f, 1f, 0.45f, 1f) : new Color(0.6f, 0.95f, 1f, 1f);
                marker.AltColor = marker.DefaultColor;
            }
            else if (GetComponentInParent<Unity.FPS.AI.EnemyController>() != null)
            {
                marker.DefaultColor = new Color(1f, 0.45f, 0.2f, 1f);
                marker.AltColor = new Color(1f, 0.1f, 0.1f, 1f);
            }

            image.color = marker.DefaultColor;
            return marker;
        }
    }
}