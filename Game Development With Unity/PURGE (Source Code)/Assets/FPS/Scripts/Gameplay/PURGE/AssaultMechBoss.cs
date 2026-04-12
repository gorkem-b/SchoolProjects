using System;
using System.Collections.Generic;
using Unity.FPS.Game;
using Unity.FPS.Gameplay;
using UnityEngine;
using UnityEngine.Animations;

namespace PURGE.Gameplay
{
    [RequireComponent(typeof(Health), typeof(Actor), typeof(Damageable))]
    public class AssaultMechBoss : MonoBehaviour
    {
        const QueryTriggerInteraction k_TriggerInteraction = QueryTriggerInteraction.Ignore;

        [Header("Boss")]
        public Transform AimPivot;
        public float DetectionRange = 220f;
        public float RotationSpeed = 75f;
        public float AttackRange = 110f;
        public float AttackDamage = 36f;
        public float AttackCooldown = 1.65f;
        public float PreferredCombatDistance = 20f;
        public float MoveLerpSpeed = 1.2f;
        public float WalkStepDistance = 5.2f;
        public float CollisionPadding = 4f;
        public Animator BossAnimator;
        public RuntimeAnimatorController MoveController;
        public RuntimeAnimatorController ShootController;
        public string MoveStateName = "Walk";
        public string[] ShootStateNames = { "ShootSmallCanon_A", "ShootSmallCanon_B" };
        public float ShootAnimationLockTime = 1.2f;

        [Header("Presentation")]
        public AudioClip SmallCanonSfx;
        public AudioClip FootstepSfx;
        public float ShotWidth = 0.14f;
        public float ShotVisualDuration = 0.1f;
        public float ShotTravelTime = 0.14f;
        public float ShotOriginForwardOffset = 3.5f;
        public float ShotOriginHeightOffset = 2.1f;
        public float ShotSfxMinDistance = 6f;
        public float SmallCanonAimErrorRadius = 0.8f;

        [ColorUsage(true, true)]
        public Color SmallCanonShotColor = new Color(0.65f, 6.2f, 9.8f, 1f);

        Health m_Health;
        PlayerCharacterController m_Player;
        Health m_PlayerHealth;
        Collider[] m_Colliders;
        LineRenderer m_ShotLine;
        float m_ShotLineDisableTime;
        float m_AttackTimer;
        float m_ReturnToMoveTime;
        bool m_AttackArmed;
        bool m_HasAggro;
        bool m_IsDead;
        bool m_UsingShootController;
        readonly List<Transform> m_ShotOrigins = new List<Transform>();
        Transform m_ShotOrigin;
        int m_ShotOriginIndex;
        Vector3 m_ShotLineStartPosition;
        Vector3 m_ShotLineTargetPosition;
        float m_ShotLineTravelStartTime;
        bool m_ShotLineTravelActive;
        Vector3 m_CurrentMoveDirection = Vector3.forward;

        void Awake()
        {
            RefreshRuntimeReferences();
            EnsureShotLine();
        }

        public void RefreshRuntimeReferences()
        {
            ApplyRuntimeTuning();

            m_Health = GetComponent<Health>();
            m_Colliders = GetComponentsInChildren<Collider>(true);

            if (AimPivot == null)
                AimPivot = transform;
            if (BossAnimator == null)
                BossAnimator = GetComponent<Animator>();

            if (BossAnimator != null)
            {
                BossAnimator.applyRootMotion = false;
                BossAnimator.enabled = false;
            }

            m_ShotOrigins.Clear();
            FindChildrenByNameContains(transform, "SmallCanon", m_ShotOrigins);

            m_ShotOrigin = m_ShotOrigins.Count > 0
                ? m_ShotOrigins[0]
                : FindChildByNameContains(transform, "SmallCanon")
                    ?? FindChildByNameContains(transform, "Small")
                    ?? FindChildByNameContains(transform, "Canon");

            m_ShotOriginIndex = 0;
        }

        void ApplyRuntimeTuning()
        {
            DetectionRange = Mathf.Max(DetectionRange, 220f);
            AttackRange = Mathf.Max(AttackRange, 110f);
            AttackDamage = Mathf.Max(AttackDamage, 42f);
            AttackCooldown = Mathf.Min(AttackCooldown, 1.2f);
            PreferredCombatDistance = Mathf.Max(PreferredCombatDistance, 20f);
            MoveLerpSpeed = Mathf.Max(MoveLerpSpeed, 6f);
            WalkStepDistance = 2.6f;
            ShootAnimationLockTime = 0.75f;
            CollisionPadding = Mathf.Max(0.8f, CollisionPadding);
        }

        void OnEnable()
        {
            if (m_Health != null)
            {
                m_Health.OnDie += OnDie;
                m_Health.OnDamaged += OnDamaged;
            }
        }

        void OnDisable()
        {
            if (m_Health != null)
            {
                m_Health.OnDie -= OnDie;
                m_Health.OnDamaged -= OnDamaged;
            }
        }

        void Start()
        {
            m_Player = FindFirstObjectByType<PlayerCharacterController>();
            m_PlayerHealth = m_Player != null ? m_Player.GetComponent<Health>() : null;
            ApplySlidingMovePose(true);
        }

        void Update()
        {
            if (m_IsDead)
            {
                UpdateShotLine();
                return;
            }

            if (m_Player == null)
            {
                m_Player = FindFirstObjectByType<PlayerCharacterController>();
                m_PlayerHealth = m_Player != null ? m_Player.GetComponent<Health>() : null;
            }
            if (m_Player == null)
                return;

            Vector3 toPlayer = m_Player.transform.position - transform.position;
            toPlayer.y = 0f;
            float playerDistance = toPlayer.magnitude;

            if (!m_HasAggro && playerDistance <= DetectionRange)
                m_HasAggro = true;

            if (m_HasAggro && toPlayer.sqrMagnitude > 0.001f)
                m_CurrentMoveDirection = toPlayer.normalized;

            if (m_HasAggro)
                AdvanceTowardsPlayer(MoveLerpSpeed * Time.deltaTime);

            if (BossAnimator != null && m_UsingShootController && Time.time >= m_ReturnToMoveTime)
                ApplySlidingMovePose();

            Vector3 aimToPlayer = m_Player.transform.position - AimPivot.position;
            aimToPlayer.y = 0f;
            Vector3 facingDirection = aimToPlayer.sqrMagnitude > 0.001f ? aimToPlayer : m_CurrentMoveDirection;
            facingDirection.y = 0f;
            if (facingDirection.sqrMagnitude > 0.001f)
            {
                Quaternion targetRotation = Quaternion.LookRotation(facingDirection.normalized, Vector3.up);
                transform.rotation = Quaternion.RotateTowards(transform.rotation, targetRotation, RotationSpeed * Time.deltaTime);
            }

            m_AttackTimer -= Time.deltaTime;
            bool canAttack = m_HasAggro && playerDistance <= AttackRange && m_AttackTimer <= 0f && HasLineOfSight(m_Player.transform);

            if (canAttack)
            {
                PlayShootAnimation();
                m_AttackTimer = AttackCooldown;
            }

            UpdateShotLine();
        }

        void OnDie()
        {
            m_IsDead = true;
            m_HasAggro = false;
            m_AttackArmed = false;
            m_ShotLineTravelActive = false;
            if (m_ShotLine != null)
                m_ShotLine.enabled = false;
            if (BossAnimator != null)
                BossAnimator.enabled = false;
        }

        void OnDamaged(float damage, GameObject damageSource)
        {
            if (m_IsDead)
                return;

            m_HasAggro = true;

            if (m_Player == null)
            {
                m_Player = FindFirstObjectByType<PlayerCharacterController>();
                m_PlayerHealth = m_Player != null ? m_Player.GetComponent<Health>() : null;
            }
        }

        // Animation event receivers used by the imported mech shoot clips.
        public void FootStep()
        {
        }
        public void EndOfWalk()
        {
        }

        public void EndOfRun()
        {
        }

        public void EndOfRunJump()
        {
        }
        public void ShootBigCanonA() { ResolveAttackShot(); }
        public void ShootBigCanonB() { ResolveAttackShot(); }
        public void ShootSmallCanonA() { ResolveAttackShot(); }
        public void ShootSmallCanonB() { ResolveAttackShot(); }

        void PlayShootAnimation()
        {
            if (BossAnimator == null || ShootController == null || ShootStateNames == null || ShootStateNames.Length == 0)
                return;

            if (!BossAnimator.enabled)
                BossAnimator.enabled = true;

            if (BossAnimator.runtimeAnimatorController != ShootController)
                BossAnimator.runtimeAnimatorController = ShootController;

            BossAnimator.applyRootMotion = false;
            SetAllLayerWeights(BossAnimator, 1f);
            BossAnimator.Rebind();
            BossAnimator.Update(0f);

            BossAnimator.speed = 1f;

            string stateName = GetSinglePhaseShootState();
            if (string.IsNullOrEmpty(stateName))
                return;

            BossAnimator.CrossFadeInFixedTime(stateName, 0.08f, 0, 0f);
            m_ReturnToMoveTime = Time.time + ShootAnimationLockTime;
            m_AttackArmed = true;
            m_UsingShootController = true;
        }

        string GetSinglePhaseShootState()
        {
            if (ShootStateNames != null)
            {
                for (int i = 0; i < ShootStateNames.Length; i++)
                {
                    string stateName = ShootStateNames[i];
                    if (!string.IsNullOrEmpty(stateName)
                        && stateName.IndexOf("SmallCanon", StringComparison.OrdinalIgnoreCase) >= 0)
                    {
                        return stateName;
                    }
                }

                for (int i = 0; i < ShootStateNames.Length; i++)
                {
                    if (!string.IsNullOrEmpty(ShootStateNames[i]))
                        return ShootStateNames[i];
                }
            }

            return null;
        }

        void ApplySlidingMovePose(bool force = false)
        {
            if (BossAnimator == null)
                return;

            if (!force && !m_UsingShootController && !BossAnimator.enabled)
                return;

            BossAnimator.applyRootMotion = false;

            if (BossAnimator.enabled)
            {
                BossAnimator.Rebind();
                BossAnimator.Update(0f);
            }

            m_AttackArmed = false;
            m_UsingShootController = false;
            BossAnimator.enabled = false;
        }

        static void SetAllLayerWeights(Animator animator, float weight)
        {
            if (animator == null)
                return;

            for (int i = 0; i < animator.layerCount; i++)
                animator.SetLayerWeight(i, weight);
        }

        void AdvanceMoveStep(float stepDistance)
        {
            if (m_CurrentMoveDirection.sqrMagnitude <= 0.0001f)
                return;

            Vector3 step = m_CurrentMoveDirection.normalized * Mathf.Max(0f, stepDistance);
            Vector3 desiredPosition = transform.position + step;
            desiredPosition = GetObstacleAwarePosition(transform.position, desiredPosition);
            transform.position = desiredPosition;
        }

        void AdvanceTowardsPlayer(float stepDistance)
        {
            if (!ShouldAdvanceTowardsPlayer())
                return;

            AdvanceMoveStep(stepDistance);
        }

        bool ShouldAdvanceTowardsPlayer()
        {
            if (!m_HasAggro || m_Player == null)
                return false;

            Vector3 toPlayer = m_Player.transform.position - transform.position;
            toPlayer.y = 0f;
            float distance = toPlayer.magnitude;
            if (distance <= Mathf.Max(1f, PreferredCombatDistance))
                return false;

            if (toPlayer.sqrMagnitude > 0.001f)
                m_CurrentMoveDirection = toPlayer.normalized;

            return true;
        }

        void ResolveAttackShot()
        {
            if (!m_AttackArmed || m_Player == null || m_PlayerHealth == null)
                return;

            m_AttackArmed = false;

            Vector3 shotOrigin = GetShotOrigin();
            Vector3 targetPoint = GetAimedTargetPoint();
            bool didHitPlayer = TryResolveShot(shotOrigin, targetPoint, out Vector3 impactPoint);

            if (didHitPlayer)
                m_PlayerHealth.TakeDamage(AttackDamage, gameObject);

            ShowShotLine(shotOrigin, impactPoint);
            PlaySfx(SmallCanonSfx, shotOrigin, ShotSfxMinDistance);
        }

        Vector3 GetShotOrigin()
        {
            Transform origin = GetNextShotOrigin();
            if (origin != null)
                return origin.position;

            Transform fallback = AimPivot != null ? AimPivot : transform;
            return fallback.position + transform.forward * ShotOriginForwardOffset + Vector3.up * ShotOriginHeightOffset;
        }

        Transform GetNextShotOrigin()
        {
            if (m_ShotOrigins.Count > 0)
            {
                Transform origin = m_ShotOrigins[m_ShotOriginIndex % m_ShotOrigins.Count];
                m_ShotOriginIndex++;
                return origin;
            }

            return m_ShotOrigin;
        }

        bool TryResolveShot(Vector3 origin, Vector3 targetPoint, out Vector3 impactPoint)
        {
            impactPoint = targetPoint;

            Vector3 direction = targetPoint - origin;
            float distance = direction.magnitude;
            if (distance <= 0.001f)
                return false;

            direction /= distance;
            if (TryGetFirstValidHit(origin, direction, distance, out RaycastHit hit))
            {
                impactPoint = hit.point;
                return IsPlayerCollider(hit.collider) || IsPlayerDamageable(hit.collider);
            }

            return false;
        }

        Vector3 GetAimedTargetPoint()
        {
            Vector3 targetPoint = m_Player.transform.position + Vector3.up * 1.35f;
            float aimErrorRadius = SmallCanonAimErrorRadius;
            if (aimErrorRadius <= 0.001f)
                return targetPoint;

            Vector2 randomOffset = UnityEngine.Random.insideUnitCircle * aimErrorRadius;
            targetPoint += m_Player.transform.right * randomOffset.x;
            targetPoint += m_Player.transform.forward * randomOffset.y;
            return targetPoint;
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

        bool IsPlayerDamageable(Collider col)
        {
            if (col == null || m_PlayerHealth == null)
                return false;

            Damageable damageable = col.GetComponentInParent<Damageable>();
            return damageable != null && damageable.Health == m_PlayerHealth;
        }

        void EnsureShotLine()
        {
            if (m_ShotLine != null)
                return;

            var shotLineObject = new GameObject("BossShotLine");
            shotLineObject.transform.SetParent(transform, false);
            m_ShotLine = shotLineObject.AddComponent<LineRenderer>();
            m_ShotLine.enabled = false;
            m_ShotLine.useWorldSpace = true;
            m_ShotLine.positionCount = 2;
            m_ShotLine.startWidth = ShotWidth;
            m_ShotLine.endWidth = ShotWidth * 0.7f;
            m_ShotLine.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
            m_ShotLine.receiveShadows = false;
            m_ShotLine.textureMode = LineTextureMode.Stretch;

            Shader lineShader = Shader.Find("Universal Render Pipeline/Unlit");
            if (lineShader == null)
                lineShader = Shader.Find("Sprites/Default");
            if (lineShader != null)
            {
                var material = new Material(lineShader);
                material.color = new Color(0.18f, 0.8f, 1f, 0.95f);
                m_ShotLine.material = material;
            }

            m_ShotLine.startColor = new Color(0.35f, 0.95f, 1f, 1f);
            m_ShotLine.endColor = new Color(0.1f, 0.5f, 1f, 0.15f);
        }

        void ShowShotLine(Vector3 start, Vector3 end)
        {
            EnsureShotLine();
            if (m_ShotLine == null)
                return;

            m_ShotLine.enabled = true;
            m_ShotLineStartPosition = start;
            m_ShotLineTargetPosition = end;
            m_ShotLineTravelStartTime = Time.time;
            m_ShotLineTravelActive = true;
            ApplyShotLineVisual(SmallCanonShotColor, ShotWidth, ShotWidth * 0.68f);
            m_ShotLine.SetPosition(0, start);
            m_ShotLine.SetPosition(1, start);
            m_ShotLineDisableTime = Time.time + ShotTravelTime + ShotVisualDuration;
        }

        void UpdateShotLine()
        {
            if (m_ShotLine != null && m_ShotLine.enabled && m_ShotLineTravelActive)
            {
                float normalizedTravel = Mathf.Clamp01((Time.time - m_ShotLineTravelStartTime) / Mathf.Max(0.01f, ShotTravelTime));
                Vector3 currentEnd = Vector3.Lerp(m_ShotLineStartPosition, m_ShotLineTargetPosition, normalizedTravel);
                m_ShotLine.SetPosition(0, m_ShotLineStartPosition);
                m_ShotLine.SetPosition(1, currentEnd);

                if (normalizedTravel >= 1f)
                    m_ShotLineTravelActive = false;
            }

            if (m_ShotLine != null && m_ShotLine.enabled && Time.time >= m_ShotLineDisableTime)
            {
                m_ShotLine.enabled = false;
                m_ShotLineTravelActive = false;
            }
        }

        void ApplyShotLineVisual(Color startColor, float startWidth, float endWidth)
        {
            if (m_ShotLine == null)
                return;

            Color endColor = startColor;
            endColor.a *= 0.18f;
            m_ShotLine.startColor = startColor;
            m_ShotLine.endColor = endColor;
            m_ShotLine.startWidth = startWidth;
            m_ShotLine.endWidth = endWidth;

            if (m_ShotLine.material != null)
                m_ShotLine.material.color = startColor;
        }

        void PlaySfx(AudioClip clip, Vector3 position, float minDistance)
        {
            if (clip == null)
                return;

            AudioUtility.CreateSFX(clip, position, AudioUtility.AudioGroups.EnemyAttack, 1f, minDistance);
        }

        Vector3 GetObstacleAwarePosition(Vector3 currentPosition, Vector3 desiredPosition)
        {
            Vector3 direction = desiredPosition - currentPosition;
            float distance = direction.magnitude;
            if (distance <= 0.001f)
                return desiredPosition;

            direction /= distance;
            RaycastHit[] hits = Physics.RaycastAll(currentPosition + Vector3.up * 2f, direction, distance + CollisionPadding, ~0, k_TriggerInteraction);
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
                return desiredPosition;

            float safeDistance = Mathf.Max(0f, nearestDistance - CollisionPadding);
            return currentPosition + direction * safeDistance;
        }

        bool HasLineOfSight(Transform target)
        {
            if (target == null)
                return false;

            Vector3 start = (AimPivot != null ? AimPivot.position : transform.position) + Vector3.up * 1.5f;
            Vector3 end = target.position + Vector3.up * 1.4f;
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

        static void FindChildrenByNameContains(Transform root, string token, List<Transform> results, string excludedToken = null)
        {
            if (root == null || string.IsNullOrEmpty(token) || results == null)
                return;

            string currentName = root.name;
            if (currentName.IndexOf(token, StringComparison.OrdinalIgnoreCase) >= 0
                && (string.IsNullOrEmpty(excludedToken) || currentName.IndexOf(excludedToken, StringComparison.OrdinalIgnoreCase) < 0))
            {
                results.Add(root);
            }

            for (int i = 0; i < root.childCount; i++)
                FindChildrenByNameContains(root.GetChild(i), token, results, excludedToken);
        }

        static Transform FindChildByNameContains(Transform root, string token)
        {
            if (root == null || string.IsNullOrEmpty(token))
                return null;

            if (root.name.IndexOf(token, StringComparison.OrdinalIgnoreCase) >= 0)
                return root;

            for (int i = 0; i < root.childCount; i++)
            {
                Transform found = FindChildByNameContains(root.GetChild(i), token);
                if (found != null)
                    return found;
            }

            return null;
        }
    }
}