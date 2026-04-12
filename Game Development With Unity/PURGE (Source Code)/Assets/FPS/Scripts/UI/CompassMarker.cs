using Unity.FPS.AI;
using Unity.FPS.Game;
using UnityEngine;
using UnityEngine.UI;

namespace Unity.FPS.UI
{
    public class CompassMarker : MonoBehaviour
    {
        [Tooltip("Main marker image")] public Image MainImage;

        [Tooltip("Canvas group for the marker")]
        public CanvasGroup CanvasGroup;

        [Header("Enemy element")] [Tooltip("Default color for the marker")]
        public Color DefaultColor;

        [Tooltip("Alternative color for the marker")]
        public Color AltColor;

        [Header("Direction element")] [Tooltip("Use this marker as a magnetic direction")]
        public bool IsDirection;

        [Tooltip("Text content for the direction")]
        public TMPro.TextMeshProUGUI TextContent;

        [Header("Guidance")]
        [Tooltip("Alpha used when the marker is clamped to the compass edge")]
        [Range(0f, 1f)]
        public float OffscreenAlpha = 0.55f;

        [Tooltip("Additional pulse scale used for active blocking objectives")]
        public float ObjectivePulseScaleMultiplier = 1.14f;

        [Tooltip("Pulse speed for active blocking objectives")]
        public float ObjectivePulseFrequency = 2.4f;

        EnemyController m_EnemyController;
        Objective m_Objective;

        public bool IsObjective => m_Objective != null;
        public bool IsBlockingObjective => m_Objective != null && m_Objective.IsBlocking();

        public void Initialize(CompassElement compassElement, string textDirection)
        {
            if (compassElement != null)
            {
                m_Objective = compassElement.transform.GetComponent<Objective>()
                    ?? compassElement.transform.GetComponentInParent<Objective>();
            }

            if (IsDirection && TextContent)
            {
                TextContent.text = textDirection;
            }
            else
            {
                m_EnemyController = compassElement.transform.GetComponent<EnemyController>()
                    ?? compassElement.transform.GetComponentInParent<EnemyController>();

                if (m_EnemyController)
                {
                    m_EnemyController.onDetectedTarget += DetectTarget;
                    m_EnemyController.onLostTarget += LostTarget;

                    LostTarget();
                }
            }
        }

        public void DetectTarget()
        {
            MainImage.color = AltColor;
        }

        public void LostTarget()
        {
            MainImage.color = DefaultColor;
        }

        public float GetAlpha(bool isOffscreen)
        {
            if (!isOffscreen)
                return 1f;

            if (IsBlockingObjective)
                return Mathf.Max(OffscreenAlpha, 0.82f);

            if (IsObjective)
                return Mathf.Max(OffscreenAlpha, 0.68f);

            if (IsDirection)
                return Mathf.Min(OffscreenAlpha, 0.45f);

            return OffscreenAlpha;
        }

        public float GetScaleMultiplier(bool isOffscreen)
        {
            float multiplier = 1f;

            if (IsBlockingObjective)
            {
                float pulse = 0.5f + 0.5f * Mathf.Sin(Time.unscaledTime * ObjectivePulseFrequency);
                multiplier *= Mathf.Lerp(1f, ObjectivePulseScaleMultiplier, pulse);
            }

            if (isOffscreen)
                multiplier *= 0.95f;

            return multiplier;
        }

        void OnDestroy()
        {
            if (m_EnemyController)
            {
                m_EnemyController.onDetectedTarget -= DetectTarget;
                m_EnemyController.onLostTarget -= LostTarget;
            }
        }
    }
}