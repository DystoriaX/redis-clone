# Redis Clone

A simple project to create a Redis clone. Tutorial is taken from https://build-your-own.org/redis/.

The request protocol is as follows:

```
set k v
get k
del k
```

The response protocol is as follows:

```
[status] [payload]
```

where `[status] = OK | BAD_REQUEST`
