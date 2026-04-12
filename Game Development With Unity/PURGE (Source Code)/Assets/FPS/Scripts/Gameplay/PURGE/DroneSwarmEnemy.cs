using System;
using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;

namespace PURGE.Gameplay
{
    [RequireComponent(typeof(Health), typeof(Actor), typeof(Damageable))]
    public class DroneSwarmEnemy : MonoBehaviour
    {
        const QueryTriggerInteraction k_TriggerInteraction = QueryTriggerInteraction.Ignore;

        [Header("Movement")]
        public Vector3 AnchorPosition;
        public float OrbitRadius = 6f;
        public float OrbitSpeed = 1.5f;
        public float HoverAmplitude = 1.25f;
        public float HoverSpeed = 2f;
        public float MoveLerpSpeed = 3f;
        public float RotationSpeed = 220f;
        public float CollisionPadding = 0.9f;
        public float MinimumGroundClearance = 2.2f;
        public float GroundProbeHeight = 6f;
        public float GroundProbeDistance = 20f;
        public float HorizontalAvoidanceHeight = 1.4f;

        [Header("Combat")]
        public float DetectionRange = 30f;
        public float AttackRange = 10f;
        public float AttackDamage = 18f;
        public float AttackCooldown = 1.25f;
        public float FollowBias = 0.35f;
        public AudioClip ShootSfx;
        public float ShootSfxMinDistance = 4f;
        public float ShotWidth = 0.08f;
        public float ShotVisualDuration = 0.06f;
        public float ImpactHeightOffset = 1.2f;

        [Header("Telegraph")]
        public float ShotTelegraphLeadTime = 0.22f;
        public float ShotTelegraphWidth = 0.05f;
        public float ShotTelegraphPulseFrequency = 10f;
        [ColorUsage(true, true)] public Color ShotTelegraphColor = new Color(0.35f, 4.2f, 8f, 1f);
        public AudioClip TelegraphSfx;
        public float TelegraphSfxMinDistance = 4f;
        public float TelegraphSfxMinInterval = 0.1f;

        public float VisualTiltAmount = 18f;
        public float DeathSpinSpeed = 540f;
        public float DeathFallSpeed = 7f;
        public float DeathShrinkSpeed = 1.5f;

        [Header("Death")]
        public float DestroyDelay = 2.5f;

        Health m_Health;
        PlayerCharacterController m_Player;
        Health m_PlayerHealth;
        Collider[] m_Colliders;
        Vector3 m_BaseScale;
        LineRenderer m_ShotLine;
        float m_ShotLineDisableTime;
        float m_ShotTelegraphReleaseTime;
        float m_ShotTelegraphStartTime;
        float m_LastTelegraphSfxTime = Mathf.NegativeInfinity;
        float m_PhaseOffset;
        float m_NextAttackTime;
        bool m_IsDead;
        bool m_ShotTelegraphActive;
        Vector3 m_ShotLineStartPosition;
        Vector3 m_ShotLineTargetPosition;

        void Awake()
        {
            m_Health = GetComponent<Health>();
            m_Colliders = GetComponentsInChildren<Collider>(true);
            m_BaseScale = transform.localScale;
            AnchorPosition = transform.position;
            m_PhaseOffset = UnityEngine.Random.Range(0f, Mathf.PI * 2f);
            EnsureShotLine();
        }

        void OnEnable()
        {
            if (m_Health != null)
                m_Health.OnDie += OnDie;
        }

        void OnDisable()
        {
            if (m_Health != null)
                m_Health.OnDie -= OnDie;
        }

        void Start()
        {
            m_Player = FindFirstObjectByType<PlayerCharacterController>();
            m_PlayerHealth = m_Player != null ? m_Player.GetComponent<Health>() : null;
        }

        void Update()
        {
            if (m_IsDead)
            {
                UpdateShotLine();
                UpdateDeathMotion();
                return;
            }

            if (m_Player == null)
            {
                m_Player = FindFirstObjectByType<PlayerCharacterController>();
                m_PlayerHealth = m_Player != null ? m_Player.GetComponent<Health>() : null;
            }

            Vector3 orbitOffset = new Vector3(
                Mathf.Cos(Time.time * OrbitSpeed + m_PhaseOffset),
                Mathf.Sin(Time.time * HoverSpeed + m_PhaseOffset) * HoverAmplitude,
                Mathf.Sin(Time.time * OrbitSpeed + m_PhaseOffset)) * OrbitRadius;

            Vector3 desiredPosition = AnchorPosition + orbitOffset;

            if (m_Player != null)
            {
                float playerDistance = Vector3.Distance(transform.position, m_Player.transform.position);
                if (playerDistance <= DetectionRange)
                {
                    Vector3 playerBiasTarget = m_Player.transform.position + Vector3.up * 2.5f;
                    desiredPosition = Vector3.Lerp(desiredPosition, playerBiasTarget, FollowBias);

                    if (playerDistance <= AttackRange && Time.time >= m_NextAttackTime && HasLineOfSight(m_Player.transform))
                    {
                        Health playerHealth = m_PlayerHealth != null ? m_PlayerHealth : m_Player.GetComponent<Health>();
                        if (playerHealth != null)
                        {
                            if (ShotTelegraphLeadTime > 0f)
                            {
                                if (!m_ShotTelegraphActive)
                                {
                                    BeginShotTelegraph();
                                }
                                else if (Time.time >= m_ShotTelegraphReleaseTime)
                                {
                                    FireAtPlayer(playerHealth);
                                    m_NextAttackTime = Time.time + AttackCooldown;
                                }
                            }
                            else
                            {
                                FireAtPlayer(playerHealth);
                                m_NextAttackTime = Time.time + AttackCooldown;
                            }
                        }
                    }
                    else if (m_ShotTelegraphActive)
                    {
                        CancelShotTelegraph();
                    }
                }
                else if (m_ShotTelegraphActive)
                {
                    CancelShotTelegraph();
                }
            }
            else if (m_ShotTelegraphActive)
            {
                CancelShotTelegraph();
            }

            UpdateShotLine();

            desiredPosition = ConstrainToGroundClearance(desiredPosition);
            desiredPosition = GetObstacleAwarePosition(transform.position, desiredPosition);
            desiredPosition = ConstrainToGroundClearance(desiredPosition);
            transform.position = Vector3.Lerp(transform.position, desiredPosition, Time.deltaTime * MoveLerpSpeed);

            Vector3 lookDirection = (desiredPosition - transform.position);
            if (m_Player != null && Vector3.Distance(transform.position, m_Player.transform.position) <= DetectionRange)
                lookDirection = m_Player.transform.position - transform.position;

            lookDirection.y = 0f;
            if (lookDirection.sqrMagnitude > 0.0001f)
            {
                Quaternion targetRotation = Quaternion.LookRotation(lookDirection.normalized, Vector3.up);
                transform.rotation = Quaternion.RotateTowards(transform.rotation, targetRotation, RotationSpeed * Time.deltaTime);

                float localTiltZ = -Mathf.Clamp(Vector3.Dot(transform.right, lookDirection.normalized) * VisualTiltAmount, -VisualTiltAmount, VisualTiltAmount);
                Vector3 euler = transform.rotation.eulerAngles;
                transform.rotation = Quaternion.Euler(euler.x, euler.y, localTiltZ);
            }
        }

        void OnDie()
        {
            if (m_IsDead)
                return;

            m_IsDead = true;
            CancelShotTelegraph();

            Collider[] colliders = GetComponentsInChildren<Collider>(true);
            foreach (Collider col in colliders)
                col.enabled = false;
        }

        void BeginShotTelegraph()
        {
            EnsureShotLine();
            if (m_ShotLine == null || m_Player == null)
                return;

            m_ShotTelegraphActive = true;
            m_ShotTelegraphStartTime = Time.time;
            m_ShotTelegraphReleaseTime = Time.time + ShotTelegraphLeadTime;
            m_ShotLineStartPosition = transform.position + transform.forward * 0.6f + Vector3.up * 0.25f;
            m_ShotLineTargetPosition = m_Player.transform.position + Vector3.up * ImpactHeightOffset;
            m_ShotLine.enabled = true;
            m_ShotLine.SetPosition(0, m_ShotLineStartPosition);
            m_ShotLine.SetPosition(1, m_ShotLineTargetPosition);

            AudioClip cue = TelegraphSfx;
            if (cue != null && Time.time >= m_LastTelegraphSfxTime + TelegraphSfxMinInterval)
            {
                AudioUtility.CreateSFX(cue, transform.position, AudioUtility.AudioGroups.EnemyAttack, 1f, TelegraphSfxMinDistance);
                m_LastTelegraphSfxTime = Time.time;
            }
        }

        void CancelShotTelegraph()
        {
            m_ShotTelegraphActive = false;
            if (m_ShotLine != null)
                m_ShotLine.enabled = false;
        }

        void UpdateDeathMotion()
        {
            UpdateShotLine();
            transform.Rotate(Vector3.up, DeathSpinSpeed * Time.deltaTime, Space.World);
            transform.Rotate(Vector3.forward, DeathSpinSpeed * 0.5f * Time.deltaTime, Space.Self);
            transform.position += Vector3.down * (DeathFallSpeed * Time.deltaTime);
            transform.localScale = Vector3.Lerp(transform.localScale, Vector3.zero, DeathShrinkSpeed * Time.deltaTime);

            if (transform.localScale.sqrMagnitude <= 0.0005f)
                Destroy(gameObject);
            else
                Destroy(gameObject, DestroyDelay);
        }

        Vector3 GetObstacleAwarePosition(Vector3 currentPosition, Vector3 desiredPosition)
        {
            Vector3 horizontalStart = currentPosition + Vector3.up * HorizontalAvoidanceHeight;
            Vector3 horizontalTarget = new Vector3(desiredPosition.x, horizontalStart.y, desiredPosition.z);
            Vector3 direction = horizontalTarget - horizontalStart;
            float distance = direction.magnitude;
            if (distance <= 0.001f)
                return ConstrainToGroundClearance(desiredPosition);

            direction /= distance;
            RaycastHit[] hits = Physics.RaycastAll(horizontalStart, direction, distance + CollisionPadding, ~0, k_TriggerInteraction);
            if (hits == null || hits.Length == 0)
                return ConstrainToGroundClearance(desiredPosition);

            float nearestDistance = float.MaxValue;
            bool blocked = false;
            for (int i = 0; i < hits.Length; i++)
            {
                Collider hitCollider = hits[i].collider;
                if (hitCollider == null || IsOwnCollider(hitCollider) || IsPlayerCollider(hitCollider))
                    continue;

                if (hits[i].distance < nearestDistance)
                {
                    nearestDistance = hits[i].distance;
                    blocked = true;
                }
            }

            if (!blocked)
                return ConstrainToGroundClearance(desiredPosition);

            float safeDistance = Mathf.Max(0f, nearestDistance - CollisionPadding);
            Vector3 safePosition = currentPosition + new Vector3(direction.x, 0f, direction.z) * safeDistance;
            safePosition.y = desiredPosition.y;
            return ConstrainToGroundClearance(safePosition);
        }

        Vector3 ConstrainToGroundClearance(Vector3 desiredPosition)
        {
            Vector3 probeStart = desiredPosition + Vector3.up * GroundProbeHeight;
            float probeDistance = GroundProbeHeight + GroundProbeDistance;
            RaycastHit[] hits = Physics.RaycastAll(probeStart, Vector3.down, probeDistance, ~0, k_TriggerInteraction);
            if (hits == null || hits.Length == 0)
                return desiredPosition;

            Array.Sort(hits, (a, b) => a.distance.CompareTo(b.distance));
            for (int i = 0; i < hits.Length; i++)
            {
                Collider hitCollider = hits[i].collider;
                if (hitCollider == null || IsOwnCollider(hitCollider) || IsPlayerCollider(hitCollider))
                    continue;

                float minY = hits[i].point.y + MinimumGroundClearance;
                if (desiredPosition.y < minY)
                    desiredPosition.y = minY;
                return desiredPosition;
            }

            return desiredPosition;
        }

        bool HasLineOfSight(Transform target)
        {
            if (target == null)
                return false;

            Vector3 start = transform.position + Vector3.up * 0.2f;
            Vector3 end = target.position + Vector3.up * 1.2f;
            Vector3 direction = end - start;
            float distance = direction.magnitude;
            if (distance <= 0.001f)
                return true;

            direction /= distance;
            if (TryGetFirstValidHit(start, direction, distance, out RaycastHit hit))
                return IsPlayerCollider(hit.collider) || IsPlayerDamageable(hit.collider);

            return false;
        }

        bool IsOwnCollider(Collider col)
        {
            if (col == null || m_Colliders == null)
                return false;

            for (int i = 0; i < m_Colliders.Length; i++)
            {
                if (m_Colliders[i] == col)
                    return true;
            }

            return false;
        }

        bool IsPlayerCollider(Collider col)
        {
            return m_Player != null && col != null && col.transform.IsChildOf(m_Player.transform);
        }

        bool IsPlayerDamageable(Collider col)
        {
            if (col == null || m_PlayerHealth == null)
                return false;

            Damageable damageable = col.GetComponentInParent<Damageable>();
            return damageable != null && damageable.Health == m_PlayerHealth;
        }

        bool TryGetFirstValidHit(Vector3 origin, Vector3 direction, float distance, out RaycastHit firstHit)
        {
            RaycastHit[] hits = Physics.RaycastAll(origin, direction, distance, ~0, k_TriggerInteraction);
            if (hits == null || hits.Length == 0)
            {
                firstHit = default;
                return false;
            }

            Array.Sort(hits, (a, b) => a.distance.CompareTo(b.distance));
            for (int i = 0; i < hits.Length; i++)
            {
                Collider hitCollider = hits[i].collider;
                if (hitCollider == null || IsOwnCollider(hitCollider))
                    continue;

                firstHit = hits[i];
                return true;
            }

            firstHit = default;
            return false;
        }

        void FireAtPlayer(Health playerHealth)
        {
            Vector3 shotOrigin = transform.position + transform.forward * 0.6f + Vector3.up * 0.25f;
            Vector3 targetPoint = m_Player.transform.position + Vector3.up * ImpactHeightOffset;
            Vector3 direction = (targetPoint - shotOrigin).normalized;
            float distance = Vector3.Distance(shotOrigin, targetPoint);

            Vector3 impactPoint = targetPoint;
            if (TryGetFirstValidHit(shotOrigin, direction, distance, out RaycastHit hit))
            {
                impactPoint = hit.point;

                Collider hitCollider = hit.collider;
                if (IsPlayerCollider(hitCollider) || IsPlayerDamageable(hitCollider))
                {
                    playerHealth.TakeDamage(AttackDamage, gameObject);
                }
            }

            ShowShotLine(shotOrigin, impactPoint);
            if (ShootSfx != null)
                AudioUtility.CreateSFX(ShootSfx, shotOrigin, AudioUtility.AudioGroups.EnemyAttack, 1f, ShootSfxMinDistance);
        }

        void EnsureShotLine()
        {
            if (m_ShotLine != null)
                return;

            var shotLineObject = new GameObject("DroneShotLine");
            shotLineObject.transform.SetParent(transform, false);
            m_ShotLine = shotLineObject.AddComponent<LineRenderer>();
            m_ShotLine.enabled = false;
            m_ShotLine.useWorldSpace = true;
            m_ShotLine.positionCount = 2;
            m_ShotLine.startWidth = ShotWidth;
            m_ShotLine.endWidth = ShotWidth * 0.55f;
            m_ShotLine.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
            m_ShotLine.receiveShadows = false;
            m_ShotLine.textureMode = LineTextureMode.Stretch;

            Shader lineShader = Shader.Find("Universal Render Pipeline/Unlit");
            if (lineShader == null)
                lineShader = Shader.Find("Sprites/Default");
            if (lineShader != null)
            {
                var material = new Material(lineShader);
                material.color = new Color(0.12f, 0.72f, 1f, 0.95f);
                m_ShotLine.material = material;
            }

            m_ShotLine.startColor = new Color(0.3f, 0.85f, 1f, 1f);
            m_ShotLine.endColor = new Color(0.1f, 0.45f, 1f, 0.1f);
        }

        void ShowShotLine(Vector3 start, Vector3 end)
        {
            EnsureShotLine();
            if (m_ShotLine == null)
                return;

            m_ShotTelegraphActive = false;
            m_ShotLineStartPosition = start;
            m_ShotLineTargetPosition = end;
            m_ShotLine.enabled = true;
            m_ShotLine.SetPosition(0, start);
            m_ShotLine.SetPosition(1, end);
            ApplyShotLineVisual(new Color(0.3f, 0.85f, 1f, 1f), ShotWidth, ShotWidth * 0.55f);
            m_ShotLineDisableTime = Time.time + ShotVisualDuration;
        }

        void UpdateShotLine()
        {
            if (m_ShotLine != null && m_ShotLine.enabled && m_ShotTelegraphActive)
            {
                m_ShotLineStartPosition = transform.position + transform.forward * 0.6f + Vector3.up * 0.25f;
                if (m_Player != null)
                    m_ShotLineTargetPosition = m_Player.transform.position + Vector3.up * ImpactHeightOffset;

                float pulse = 0.5f + 0.5f * Mathf.Sin((Time.time - m_ShotTelegraphStartTime) * ShotTelegraphPulseFrequency);
                ApplyShotLineVisual(ShotTelegraphColor * Mathf.Lerp(0.8f, 1.15f, pulse),
                    ShotTelegraphWidth * Mathf.Lerp(0.85f, 1.2f, pulse),
                    ShotTelegraphWidth * Mathf.Lerp(0.45f, 0.72f, pulse));
                m_ShotLine.SetPosition(0, m_ShotLineStartPosition);
                m_ShotLine.SetPosition(1, m_ShotLineTargetPosition);
                return;
            }

            if (m_ShotLine != null && m_ShotLine.enabled && Time.time >= m_ShotLineDisableTime)
                m_ShotLine.enabled = false;
        }

        void ApplyShotLineVisual(Color startColor, float startWidth, float endWidth)
        {
            if (m_ShotLine == null)
                return;

            Color endColor = startColor;
            endColor.a *= 0.15f;
            m_ShotLine.startColor = startColor;
            m_ShotLine.endColor = endColor;
            m_ShotLine.startWidth = startWidth;
            m_ShotLine.endWidth = endWidth;

            if (m_ShotLine.material != null)
                m_ShotLine.material.color = startColor;
        }
    }
}