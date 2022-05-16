# ptrace_experiments
Experimenting shell injection with `ptrace`.


## Steps
1. Build docker image `ptrace-exp`:
```shell
docker build . --tag ptrace-exp
```

2. Run image `ptrace-exp`:
```shell
docker run --name ptrace-1 --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -it ptrace-exp  bash
```

3. In container, run `tracee`, copy the pid printed:
```shell
(docker) ./tracee
```

4. From another session, access the same container from the outside:
```shell
docker exec -it ptrace-1 /bin/bash
```

5. In container, run `tracer` to inject:
```shell
(docker) ./tracer <pid>
```

6. Now we should be able to run arbitrary commands from the first session.
