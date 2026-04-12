using Unity.FPS.Game;

namespace PURGE.Gameplay
{
    public class BossDefeatObjective : Objective
    {
        public void MarkBossDefeated()
        {
            if (IsCompleted)
                return;

            CompleteObjective(Description, "1 / 1", "Objective complete : " + Title);
        }
    }
}