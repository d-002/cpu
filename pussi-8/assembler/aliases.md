## List of aliases

- `ldi data,destination`
    ```
    ldi data
    movei %r0, destination
    ```

- `jeq target`
    ```
    cond 128
    jump target
    ```

- `jne target`
    ```
    cond 129
    jump target
    ```

- `jca target`
    ```
    cond 64
    jump target
    ```

- `jnc target`
    ```
    cond 65
    jump target
    ```

- `jpl target`
    ```
    cond 33
    jump target
    ```

- `jmi target`
    ```
    cond 32
    jump target
    ```

- `jvs target`
    ```
    cond 17
    jump target
    ```

- `jvc target`
    ```
    cond 16
    jump target
    ```

- `jos target`
    ```
    cond 9
    jump target
    ```

- `joc target`
    ```
    cond 8
    jump target
    ```

- `jus target`
    ```
    cond 5
    jump target
    ```

- `juc target`
    ```
    cond 4
    jump target
    ```

- `jmp target`
    ```
    cond 2
    jump target
    ```
