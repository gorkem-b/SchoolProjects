export const paragraphs = [
  'Consistency beats intensity. Write at a steady rhythm and let accuracy lead your speed.',
  'A focused minute of clean typing builds better muscle memory than ten rushed attempts.',
  'Small daily progress turns into strong reflexes when your hands trust the keyboard.',
  'Speed follows control. Keep your eyes on the text and your fingers close to home row.',
  'Every correct character is a signal to your brain that precision comes first and pace follows.',
  'Breathe, relax your shoulders, and type smoothly. Tension slows your hands more than distance.',
  'Train with intention: reduce errors, maintain flow, and let your words arrive with confidence.',
  'The best typists are calm under pressure. They recover quickly and keep moving forward.',
  'When you miss a key, reset instantly. One mistake matters less than your next ten characters.',
  'Typing is rhythm plus attention. Hear the cadence and keep each stroke deliberate.',
  'Accuracy creates speed over time. Build clean habits first, then increase tempo naturally.',
  'Keep your wrists neutral and your movements small to reduce fatigue and improve control.',
  'A short, focused practice session can improve confidence more than a long distracted one.',
  'In typing, recovery matters. Return to rhythm quickly after mistakes and continue smoothly.',
  'High performance is usually quiet and controlled, not frantic or forceful.',
  'Practice turns thought into reflex, and reflex turns effort into flow.',
  'Strong fundamentals make difficult passages feel easier under pressure.',
  'Typing well is a balance: vision, rhythm, and precise finger placement.',
  'Measure progress with both speed and accuracy to avoid building unstable habits.',
  'Each deliberate keystroke trains consistency, and consistency compounds into speed.',
  'Calm breathing and relaxed shoulders often improve timing more than extra force.',
  'Mistakes are feedback. Correct your pattern and keep moving without frustration.',
  'A reliable pace over sixty seconds beats a fast start with frequent breakdowns.',
  'Typing confidence grows from repetition, clarity, and a stable hand position.',
  'Keep your focus on the current character and let rhythm guide the next one.',
  'Precision in early practice prevents bad habits from becoming automatic.',
  'Skilled typists maintain flow even when the text becomes unfamiliar or complex.',
  'Good posture supports endurance, and endurance supports reliable performance.',
  'Control your tempo instead of chasing speed spikes that increase error rates.',
  'When your attention is stable, your hands become faster without extra tension.',
  'Write cleanly, then quickly. That sequence builds sustainable improvement.',
  'Typing is a physical skill shaped by repetition, intention, and steady feedback.',
  'Use each run to refine one thing: rhythm, accuracy, posture, or recovery.',
  'Progress is often invisible day to day but obvious over consistent weeks.',
  'A smooth cadence helps your fingers anticipate movement and reduce hesitation.',
  'Errors are normal; lingering on them is optional. Reset and continue.',
  'Discipline in short sessions can outperform motivation in long inconsistent ones.',
  'Strong keyboard awareness reduces visual strain and improves confidence.',
  'The most effective practice is focused, repeatable, and easy to sustain.',
  'Reliable typing comes from habits you can reproduce under any condition.',
  'Speed is the result of clean execution repeated over time.',
  'Focus on the process, and the metrics will gradually follow.',
  'Every session is a chance to improve one percent with intention.',
  'Stay relaxed, stay present, and let precision shape your tempo.',
  'Even small corrections to hand position can improve long-term control.',
  'A stable rhythm reduces mental load and keeps your output consistent.',
  'Type with purpose, not panic, and your results will be more durable.',
  'Well-trained movement is efficient, accurate, and surprisingly calm.',
  'The best momentum comes from confidence built on reliable fundamentals.',
  'Skill compounds when practice is regular, measurable, and patient.',
]

export function getRandomParagraph(exclude = '') {
  // Keep paragraph rotation smooth by avoiding immediate repetition.
  if (paragraphs.length === 0) return ''
  if (paragraphs.length === 1) return paragraphs[0]

  let next = paragraphs[Math.floor(Math.random() * paragraphs.length)]
  while (next === exclude) {
    next = paragraphs[Math.floor(Math.random() * paragraphs.length)]
  }

  return next
}
