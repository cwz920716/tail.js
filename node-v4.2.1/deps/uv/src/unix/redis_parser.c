typedef struct redisReader {
    int err; /* Error flags, 0 when there is no error */
    char *buf; /* Read buffer */
    size_t pos; /* Buffer cursor */
    size_t len; /* Buffer length */
    size_t maxbuf; /* Max length of unused buffer */
} redisReader;
static int hasLeft = 0;

/* Find pointer to \r\n. */
static char *seekNewline(char *s, size_t len) {
    int pos = 0;
    int _len = len-1;

    /* Position should be < len-1 because the character at "pos" should be
     * followed by a \n. Note that strchr cannot be used because it doesn't
     * allow to search a limited length and the buffer that is being searched
     * might not have a trailing NULL character. */
    while (pos < _len) {
        while(pos < _len && s[pos] != '\r') pos++;
        if (s[pos] != '\r') {
            /* Not found. */
            return NULL;
        } else {
            if (s[pos+1] == '\n') {
                /* Found. */
                return s+pos;
            } else {
                /* Continue searching. */
                pos++;
            }
        }
    }
    return NULL;
}

static char *readLine(redisReader *r, int *_len) {
    char *p, *s;
    int len;

    p = r->buf+r->pos;
    s = seekNewline(p,(r->len-r->pos));
    if (s != NULL) {
        len = s-(r->buf+r->pos);
        r->pos += len+2; /* skip \r\n */
        if (_len) *_len = len;
        return p;
    }
    return NULL;
}

static int processLineItem(redisReader *r) {
    char *p;
    int len;

    if ((p = readLine(r,&len)) != NULL) {
        return REDIS_OK;
    }

    return REDIS_ERR;
}

static int processBulkItem(redisReader *r) {
    char *p, *s;
    long len;
    unsigned long bytelen;
    int success = 0;

    p = r->buf+r->pos;
    s = seekNewline(p,r->len-r->pos);
    if (s != NULL) {
        p = r->buf+r->pos;
        bytelen = s-(r->buf+r->pos)+2; /* include \r\n */
        len = readLongLong(p);

        if (len < 0) {
            success = 1;
        } else {
            /* Only continue when the buffer contains the entire bulk item. */
            bytelen += len+2; /* include \r\n */
            if (r->pos+bytelen <= r->len) {
                if (r->fn && r->fn->createString)
                    obj = r->fn->createString(cur,s+2,len);
                else
                    obj = (void*)REDIS_REPLY_STRING;
                success = 1;
            }
        }

        /* Proceed when obj was created. */
        if (success) {
            r->pos += bytelen;
            return REDIS_OK;
        }
    }

    return REDIS_ERR;
}


static char *processItem(char *s, int length) {
    int pos = 0;
    char p* = s;

    switch (p[0]) {
        case '-':
            cur->type = REDIS_REPLY_ERROR;
            break;
        case '+':
            cur->type = REDIS_REPLY_STATUS;
            break;
        case ':':
            cur->type = REDIS_REPLY_INTEGER;
            break;
        case '$':
            cur->type = REDIS_REPLY_STRING;
            break;
        case '*':
            cur->type = REDIS_REPLY_ARRAY;
            break;
        default:
            return REDIS_ERR;
    }

    switch(cur->type) {
        case REDIS_REPLY_ERROR:
        case REDIS_REPLY_STATUS:
        case REDIS_REPLY_INTEGER:
            return processLineItem(s, length);
        case REDIS_REPLY_STRING:
            return processStringItem(r);
        case REDIS_REPLY_ARRAY:
            return processArrayItem(r);
        default:
            assert(NULL);
            return REDIS_ERR; /* Avoid warning. */
    }
}

/* returns the number of complete response from redis server */
int scan(char *buf, int length)
{
    if (hasLeft) {
        // do something
    }

    
}
