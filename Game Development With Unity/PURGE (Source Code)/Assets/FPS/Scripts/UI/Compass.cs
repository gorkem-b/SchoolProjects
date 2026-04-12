using System.Collections.Generic;
using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;

namespace Unity.FPS.UI
{
    public class Compass : MonoBehaviour
    {
        public RectTransform CompasRect;
        public float VisibilityAngle = 180f;
        public float HeightDifferenceMultiplier = 2f;
        public float MinScale = 0.5f;
        public float DistanceMinScale = 50f;
        public float CompasMarginRatio = 0.8f;

        [Tooltip("If true, markers stay visible by clamping to the compass edges when outside the front arc")]
        public bool ClampMarkersToEdges = true;

        [Tooltip("Extra scale multiplier applied to clamped markers")]
        public float EdgeMarkerScaleMultiplier = 0.95f;

        public GameObject MarkerDirectionPrefab;

        Transform m_PlayerTransform;
        Dictionary<Transform, CompassMarker> m_ElementsDictionnary = new Dictionary<Transform, CompassMarker>();
        readonly List<Transform> m_StaleElements = new List<Transform>();

        float m_WidthMultiplier;
        float m_HeightOffset;

        void Awake()
        {
            PlayerCharacterController playerCharacterController = FindFirstObjectByType<PlayerCharacterController>();
            DebugUtility.HandleErrorIfNullFindObject<PlayerCharacterController, Compass>(playerCharacterController,
                this);
            m_PlayerTransform = playerCharacterController.transform;

            m_WidthMultiplier = CompasRect.rect.width / VisibilityAngle;
            m_HeightOffset = -CompasRect.rect.height / 2;
        }

        void Update()
        {
            // this is all very WIP, and needs to be reworked
            m_StaleElements.Clear();

            foreach (var element in m_ElementsDictionnary)
            {
                if (element.Key == null || element.Value == null || element.Value.CanvasGroup == null)
                {
                    m_StaleElements.Add(element.Key);
                    continue;
                }

                float distanceRatio = 1;
                float heightDifference = 0;
                float angle;

                if (element.Value.IsDirection)
                {
                    angle = Vector3.SignedAngle(m_PlayerTransform.forward,
                        element.Key.transform.localPosition.normalized, Vector3.up);
                }
                else
                {
                    Vector3 targetDir = (element.Key.transform.position - m_PlayerTransform.position).normalized;
                    targetDir = Vector3.ProjectOnPlane(targetDir, Vector3.up);
                    Vector3 playerForward = Vector3.ProjectOnPlane(m_PlayerTransform.forward, Vector3.up);
                    angle = Vector3.SignedAngle(playerForward, targetDir, Vector3.up);

                    Vector3 directionVector = element.Key.transform.position - m_PlayerTransform.position;

                    heightDifference = (directionVector.y) * HeightDifferenceMultiplier;
                    heightDifference = Mathf.Clamp(heightDifference, -CompasRect.rect.height / 2 * CompasMarginRatio,
                        CompasRect.rect.height / 2 * CompasMarginRatio);

                    distanceRatio = directionVector.magnitude / DistanceMinScale;
                    distanceRatio = Mathf.Clamp01(distanceRatio);
                }

                float halfVisibility = VisibilityAngle * 0.5f;
                bool isOffscreen = angle <= -halfVisibility || angle >= halfVisibility;
                float edgeHalfAngle = halfVisibility * Mathf.Max(0.01f, CompasMarginRatio);
                float displayedAngle = ClampMarkersToEdges ? Mathf.Clamp(angle, -edgeHalfAngle, edgeHalfAngle) : angle;
                float markerX = m_WidthMultiplier * displayedAngle;
                float scale = Mathf.Lerp(1, MinScale, distanceRatio) * element.Value.GetScaleMultiplier(isOffscreen);
                if (isOffscreen)
                    scale *= EdgeMarkerScaleMultiplier;

                if (!isOffscreen || ClampMarkersToEdges)
                {
                    element.Value.CanvasGroup.alpha = element.Value.GetAlpha(isOffscreen);
                    element.Value.CanvasGroup.transform.localPosition = new Vector2(markerX,
                        heightDifference + m_HeightOffset);
                    element.Value.CanvasGroup.transform.localScale = Vector3.one * scale;
                }
                else
                {
                    element.Value.CanvasGroup.alpha = 0;
                }
            }

            for (int i = 0; i < m_StaleElements.Count; i++)
            {
                Transform stale = m_StaleElements[i];
                if (stale != null && m_ElementsDictionnary.TryGetValue(stale, out CompassMarker marker) && marker != null && marker.CanvasGroup != null)
                    Destroy(marker.CanvasGroup.gameObject);
                m_ElementsDictionnary.Remove(stale);
            }
        }

        public void RegisterCompassElement(Transform element, CompassMarker marker)
        {
            if (element == null || marker == null)
                return;

            marker.transform.SetParent(CompasRect);

            if (m_ElementsDictionnary.ContainsKey(element))
                m_ElementsDictionnary[element] = marker;
            else
                m_ElementsDictionnary.Add(element, marker);
        }

        public void UnregisterCompassElement(Transform element)
        {
            if (m_ElementsDictionnary.TryGetValue(element, out CompassMarker marker) && marker.CanvasGroup != null)
                Destroy(marker.CanvasGroup.gameObject);
            m_ElementsDictionnary.Remove(element);
        }
    }
}