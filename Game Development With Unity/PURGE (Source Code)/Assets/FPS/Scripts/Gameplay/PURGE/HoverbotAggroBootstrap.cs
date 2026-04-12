using Unity.FPS.AI;
using Unity.FPS.Gameplay;
using UnityEngine;
using UnityEngine.AI;

namespace PURGE.Gameplay
{
    [RequireComponent(typeof(EnemyController))]
    public class HoverbotAggroBootstrap : MonoBehaviour
    {
        public float AggroRefreshInterval = 0.1f;
        public float NavMeshRecoveryRadius = 18f;
        public float DirectChaseSpeed = 22f;
        public float DirectChaseProbeHeight = 1.4f;
        public float DirectChaseObstaclePadding = 0.9f;

        EnemyController m_EnemyController;
        DetectionModule m_DetectionModule;
        EnemyMobile m_EnemyMobile;
        PlayerCharacterController m_Player;
        NavMeshAgent m_NavMeshAgent;
        float m_NextAggroTime;

        void Awake()
        {
            m_EnemyController = GetComponent<EnemyController>();
            m_DetectionModule = GetComponentInChildren<DetectionModule>(true);
            m_EnemyMobile = GetComponent<EnemyMobile>();
            m_NavMeshAgent = GetComponent<NavMeshAgent>();
        }

        void Update()
        {
            if (m_EnemyController == null || m_DetectionModule == null)
                return;

            if (m_Player == null)
                m_Player = FindFirstObjectByType<PlayerCharacterController>();
            if (m_Player == null)
                return;

            bool navReady = HasReadyNavMeshAgent();

            if (Time.time >= m_NextAggroTime)
            {
                navReady = EnsureNavMeshAgentReady();
                EnsureAggroState();

                if (navReady)
                    m_EnemyController.SetNavDestination(m_Player.transform.position);

                m_NextAggroTime = Time.time + AggroRefreshInterval;
            }

            if (!navReady)
                MoveDirectlyTowardsPlayer();
        }

        void EnsureAggroState()
        {
            if (m_Player == null)
                return;

            if (m_EnemyController.KnownDetectedTarget == null)
                m_DetectionModule.OnDamaged(m_Player.gameObject);

            if (m_EnemyMobile != null && m_EnemyMobile.AiState == EnemyMobile.AIState.Patrol)
                m_DetectionModule.OnDetect();
        }

        void MoveDirectlyTowardsPlayer()
        {
            if (m_Player == null)
                return;

            Vector3 playerPosition = m_Player.transform.position;
            Vector3 flatTarget = new Vector3(playerPosition.x, transform.position.y, playerPosition.z);
            Vector3 toTarget = flatTarget - transform.position;
            float stopDistance = Mathf.Max(1.5f, m_DetectionModule.AttackRange * 0.6f);

            if (toTarget.sqrMagnitude > stopDistance * stopDistance)
            {
                float speed = m_NavMeshAgent != null && m_NavMeshAgent.speed > 0f ? m_NavMeshAgent.speed : DirectChaseSpeed;
                float maxStepDistance = Mathf.Min(speed * Time.deltaTime, toTarget.magnitude - stopDistance);
                Vector3 direction = toTarget.normalized;

                if (TryGetSafeDirectChaseDistance(direction, maxStepDistance, out float safeDistance))
                    maxStepDistance = Mathf.Min(maxStepDistance, safeDistance);

                if (maxStepDistance > 0.001f)
                    transform.position += direction * maxStepDistance;
            }

            m_EnemyController.OrientTowards(playerPosition);
            m_EnemyController.OrientWeaponsTowards(playerPosition);
        }

        bool TryGetSafeDirectChaseDistance(Vector3 direction, float requestedDistance, out float safeDistance)
        {
            safeDistance = requestedDistance;
            if (requestedDistance <= 0.001f)
                return false;

            Vector3 origin = transform.position + Vector3.up * DirectChaseProbeHeight;
            RaycastHit[] hits = Physics.RaycastAll(origin, direction, requestedDistance + DirectChaseObstaclePadding, ~0,
                QueryTriggerInteraction.Ignore);

            float nearestHitDistance = float.MaxValue;
            bool blocked = false;
            for (int i = 0; i < hits.Length; i++)
            {
                Collider hitCollider = hits[i].collider;
                if (hitCollider == null)
                    continue;

                if (hitCollider.transform.IsChildOf(transform))
                    continue;

                if (m_Player != null && hitCollider.transform.IsChildOf(m_Player.transform))
                    continue;

                if (hits[i].distance < nearestHitDistance)
                {
                    nearestHitDistance = hits[i].distance;
                    blocked = true;
                }
            }

            if (!blocked)
                return false;

            safeDistance = Mathf.Max(0f, nearestHitDistance - DirectChaseObstaclePadding);
            return true;
        }

        bool HasReadyNavMeshAgent()
        {
            return m_NavMeshAgent != null && m_NavMeshAgent.enabled && m_NavMeshAgent.isOnNavMesh;
        }

        bool EnsureNavMeshAgentReady()
        {
            if (m_NavMeshAgent == null)
                return false;

            if (m_NavMeshAgent.enabled && m_NavMeshAgent.isOnNavMesh)
            {
                m_NavMeshAgent.isStopped = false;
                return true;
            }

            NavMeshHit navHit;
            bool foundNavMesh =
                NavMesh.SamplePosition(transform.position, out navHit, NavMeshRecoveryRadius, NavMesh.AllAreas) ||
                NavMesh.SamplePosition(transform.position + Vector3.down * 4f, out navHit, NavMeshRecoveryRadius, NavMesh.AllAreas) ||
                NavMesh.SamplePosition(transform.position + Vector3.up * 2f, out navHit, NavMeshRecoveryRadius, NavMesh.AllAreas);

            if (!foundNavMesh)
                return false;

            float hoverOffset = Mathf.Max(m_NavMeshAgent.baseOffset, transform.position.y - navHit.position.y);

            if (!m_NavMeshAgent.enabled)
                m_NavMeshAgent.enabled = true;

            if (!m_NavMeshAgent.Warp(navHit.position) && !m_NavMeshAgent.isOnNavMesh)
            {
                m_NavMeshAgent.enabled = false;
                return false;
            }

            m_NavMeshAgent.baseOffset = hoverOffset;
            m_NavMeshAgent.nextPosition = navHit.position;
            m_NavMeshAgent.isStopped = false;
            return m_NavMeshAgent.isOnNavMesh;
        }
    }
}
