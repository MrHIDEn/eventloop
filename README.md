# eventloop

Cooperative timer + async-task executor for
[Klin](https://github.com/klin-lang/klin) (issue 029).

No heap `Promise`, no hidden malloc, no hidden scheduler. Callbacks and
`async fn` state machines share one explicit `run()`.

## Install

```sh
klin get github/klin-lang/eventloop@v0.4.0
```

```klin
import "github/klin-lang/eventloop"
```

## `$event_loop` (v0.3+)

Same direction as `$rtos_task` in `klin_freertos` — ergonomics in the library,
explicit expand (`--emit-pp`), no hidden scheduler.

```klin
import "github/klin-lang/eventloop"
import io

struct App {
    ticks: i32
    ex: *mut u8
}

fn on_tick(ctx: *mut u8) {
    let app = cast(*mut App, ctx)
    (*app).ticks = (*app).ticks + 1
    io.println("tick")
    if (*app).ticks >= 3 {
        eventloop.stop(cast(*mut eventloop.Executor, (*app).ex))
    }
}

fn main() {
    $event_loop(ex) {
        let mut app = App{ ticks: 0, ex: cast(*mut u8, &ex) }
        let _ = eventloop.every_ms(&ex, 100, on_tick, cast(*mut u8, &app)) or { 0 }
    }
}
```

Expand shape (`$mod` → import qualifier):

```klin
let mut ex: eventloop.Executor
let rc_ex = eventloop.init(&ex) or { 1 }
if rc_ex != 0 {
  return
}
// body…
eventloop.run(&ex)
```

Nestable inside `$rtos_task` — one `Executor` per call site, not a global Node loop.

See [`examples/event_loop_macro/`](examples/event_loop_macro/).

## Callback example (manual API)

```klin
import "github/klin-lang/eventloop"
import io

struct App {
    ticks: i32
    ex: *mut u8
}

fn on_tick(ctx: *mut u8) {
    let app = cast(*mut App, ctx)
    (*app).ticks = (*app).ticks + 1
    io.println("tick")
    if (*app).ticks >= 3 {
        eventloop.stop(cast(*mut eventloop.Executor, (*app).ex))
    }
}

fn main() {
    let mut ex: eventloop.Executor
    let mut app = App{ ticks: 0, ex: cast(*mut u8, &ex) }
    let _ = eventloop.init(&ex) or { 1 }
    let _ = eventloop.every_ms(&ex, 100, on_tick, cast(*mut u8, &app)) or { 0 }
    eventloop.run(&ex)
}
```

## Async example (needs Klin with `async` / `.await`)

```klin
import "github/klin-lang/eventloop"
import io

async fn delay_ms(ms: i64) {
    eventloop.sleep_ms(ms).await
}

async fn ticker() {
    let mut n: i32 = 0
    while n < 3 {
        io.println("tick")
        n = n + 1
        delay_ms(50).await
    }
}

fn main() {
    $event_loop(ex) {
        let _ = eventloop.spawn(&ex, ticker) or { 1 }
    }
}
```

## `flag_wait` (v0.4) — ISR / producer → `.await`

One-shot flag: producer (ISR or timer) calls `flag_set`; an `async fn` suspends on
`flag_wait(f).await`. Continuation happens when `run()` **polls** again — not a
JS Promise resolve and not a push-wake from the IRQ.

```klin
// ISR or synthetic producer — set only; never await / run / spawn
eventloop.flag_set(&flag)

async fn reader() {
    eventloop.flag_wait(&flag).await
    // work outside ISR
}
```

- Auto-reset on Ready (`poll` clears the flag).
- Accessors use `*mut volatile i32` (Klin has no `volatile` struct fields).
- Host smoke: [`examples/flag_wait/`](examples/flag_wait/).

On FreeRTOS, use [`klin_freertos`](https://github.com/klin-lang/klin_freertos)
FromISR (`semaphore_give_from_isr` + `task_yield_from_isr`) to wake the task that
owns `run()` if you care about idle/power — `flag_wait` does **not** replace that.

## Not in this release

- Value-returning `.await` / typed channel (`recv().await → T`)
- JS-style Promise / microtask queue / hidden global loop
- Waker / push-wake from ISR (ISR only sets the flag)
- WFI / `nanosleep` / low-power idle while Pending on a flag (busy-poll today)
- FreeRTOS task wake from ISR (use `klin_freertos` FromISR)
- Sticky / level-triggered flag (MVP = auto-reset one-shot)
- `@[isr]` on `async fn` (forbidden — ISR and async stay separate)
- Atomics / memory-order API beyond `volatile` 0/1
- Multi-waiter on one flag (undefined)
- Board demo with a real IRQ (stub/host only; hardware → Klin 028/030)

## API

| Function / macro | Role |
|---|---|
| `version(): i32` | package version (`4` for v0.4.0) |
| `$event_loop(ex) { … }` | `let mut ex` + `init` + body + `run` |
| `init(ex): !i32` | reset 16 timer slots + 8 task slots |
| `every_ms` / `once_ms` / `cancel` / `stop` | callback timers |
| `sleep_ms(ms): SleepFuture` | awaitable deadline (`poll` → 0/1) |
| `flag_init` / `flag_set` / `flag_clear` / `flag_wait` | one-shot flag + `FlagFuture` |
| `spawn(ex, poll, init): !i32` | task slot; Klin sugar `spawn(&ex, async_fn)` |
| `run(ex)` | due callbacks **and** `poll` of async tasks |

- Timer capacity: 16 slots. Task capacity: 8 × 256-byte state buffers.
- Host idle: busy-wait on `time.mono()` (no `nanosleep` yet).
- Not for freestanding MCU without host `time`.

## Layout

```text
eventloop/
  version.kl
  executor.kl          # API + $fn event_loop
  executor_test.kl     # klin test (skipped on import)
examples/
  event_loop_macro/    # $event_loop expand / run check
  flag_wait/           # flag_set + flag_wait().await (host)
```

## Versioning

Pins are git tags (`v0.1.0` callbacks, `v0.2.0` async poll, `v0.3.0` `$event_loop`,
`v0.4.0` `flag_wait`, …).
