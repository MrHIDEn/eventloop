# `$event_loop` macro

Expands to an explicit `Executor` + `init` + body + `run`.

Requires a Klin build that supports:
- `$fn` `block` parameters (`$name(args) { … }`)
- `$fn` export via `import "…"` (path imports); `$mod` → import qualifier

```sh
make KLIN=/path/to/klin/bin/klin.dart
make pp KLIN=/path/to/klin/bin/klin.dart   # peek expansion
```

Before expand:

```klin
$event_loop(ex) {
    let _ = eventloop.every_ms(&ex, 50, on_tick, ctx) or { 0 }
}
```

After expand (shape):

```klin
let mut ex: eventloop.Executor
let rc_ex = eventloop.init(&ex) or { 1 }
if rc_ex != 0 {
  return
}
let _ = eventloop.every_ms(&ex, 50, on_tick, ctx) or { 0 }
eventloop.run(&ex)
```
