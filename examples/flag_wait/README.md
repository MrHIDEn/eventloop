# flag_wait (host smoke)

Shows **ISR → flag → `.await`** without a board:

1. `once_ms` acts as a synthetic IRQ and calls `flag_set` only
2. `async fn reader` suspends on `flag_wait(…).await`
3. `run()` polls until Ready (auto-reset), then continues

Shared `Flag` lives in C BSS (`flag_cell.c`) because Klin has no module-level
variables yet. On a real MCU, use `@[isr("Vector_Handler")]` + `flag_set` the
same way (issue 030). For FreeRTOS idle/power, still use FromISR semaphores
(`klin_freertos` @v0.3.0) — `flag_wait` does not replace them.

```sh
make KLIN=/path/to/klin/bin/klin.dart
```
