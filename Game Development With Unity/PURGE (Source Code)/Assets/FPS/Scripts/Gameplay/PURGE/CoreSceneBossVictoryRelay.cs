using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;

namespace PURGE.Gameplay
{
    [RequireComponent(typeof(Health))]
    public class CoreSceneBossVictoryRelay : MonoBehaviour
    {
        public BossDefeatObjective Objective;

        Health m_Health;
        bool m_HasTriggered;

        void Awake()
        {
            m_Health = GetComponent<Health>();
            if (m_Health != null)
            {
                m_Health.OnDie += OnBossDied;
            }
        }

        void OnDestroy()
        {
            if (m_Health != null)
            {
                m_Health.OnDie -= OnBossDied;
            }
        }

        void OnBossDied()
        {
            if (m_HasTriggered)
                return;

            m_HasTriggered = true;

            if (Objective != null && !Objective.IsCompleted)
            {
                Objective.MarkBossDefeated();
            }
        }
    }
}