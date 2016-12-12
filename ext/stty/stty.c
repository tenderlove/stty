#include <ruby.h>
#include <ruby/io.h>
#include <termios.h>

VALUE mStty;
VALUE cSttyTermios;

static const rb_data_type_t stty_termios_type = {
    "Stty/Termios",
    {0, xfree, 0,},
    0, 0,
#ifdef RUBY_TYPED_FREE_IMMEDIATELY
    RUBY_TYPED_FREE_IMMEDIATELY,
#endif
};

static VALUE allocate(VALUE klass)
{
    struct termios *tty;

    tty = xcalloc(1, sizeof(struct termios));

    return TypedData_Make_Struct(klass, struct termios, &stty_termios_type, tty);
}

static VALUE rb_tcgetattr(VALUE self, VALUE io)
{
    struct termios *tty;
    rb_io_t *fptr;

    TypedData_Get_Struct(self, struct termios, &stty_termios_type, tty);
    GetOpenFile(io, fptr);

    if (tcgetattr(fptr->fd, tty) != 0) {
	rb_raise(rb_eRuntimeError, "error %d", errno);
    }

    return self;
}

static VALUE rb_tcsetattr(VALUE self, VALUE io, VALUE attrs)
{
    struct termios *tty;
    rb_io_t *fptr;

    TypedData_Get_Struct(self, struct termios, &stty_termios_type, tty);
    GetOpenFile(io, fptr);

    if (tcsetattr(fptr->fd, NUM2INT(attrs), tty) != 0) {
	rb_raise(rb_eRuntimeError, "error %d", errno);
    }

    return self;
}

static VALUE rb_cfsetspeed(VALUE self, VALUE val)
{
    struct termios *tty;
    TypedData_Get_Struct(self, struct termios, &stty_termios_type, tty);

    cfsetspeed(tty, NUM2INT(val));

    return self;
}

static VALUE rb_cfgetispeed(VALUE self)
{
    struct termios *tty;
    TypedData_Get_Struct(self, struct termios, &stty_termios_type, tty);

    return INT2NUM(cfgetispeed(tty));
}

static VALUE rb_cfgetospeed(VALUE self)
{
    struct termios *tty;
    TypedData_Get_Struct(self, struct termios, &stty_termios_type, tty);

    return INT2NUM(cfgetospeed(tty));
}

static VALUE rb_tcflush(VALUE self, VALUE io, VALUE when)
{
    struct termios *tty;
    rb_io_t *fptr;

    GetOpenFile(io, fptr);
    TypedData_Get_Struct(self, struct termios, &stty_termios_type, tty);

    tcflush(fptr->fd, NUM2INT(when));

    return when;
}

static VALUE rb_get_c_cc(VALUE self, VALUE i)
{
    struct termios *tty;
    int idx = NUM2INT(i);

    if (idx >= NCCS) {
	rb_raise(rb_eRuntimeError, "index is wrong %d vs %d", idx, NCCS);
    }

    TypedData_Get_Struct(self, struct termios, &stty_termios_type, tty);

    return INT2NUM(tty->c_cc[idx]);
}

static VALUE rb_set_c_cc(VALUE self, VALUE i, VALUE val)
{
    struct termios *tty;
    int idx = NUM2INT(i);

    if (idx >= NCCS) {
	rb_raise(rb_eRuntimeError, "index is wrong %d vs %d", idx, NCCS);
    }

    TypedData_Get_Struct(self, struct termios, &stty_termios_type, tty);

    tty->c_cc[idx] = NUM2INT(val);

    return val;
}

#define TC_FLAG_ATTR(name) \
    static VALUE rb_get_##name(VALUE self) \
{ \
    struct termios *tty; \
    TypedData_Get_Struct(self, struct termios, &stty_termios_type, tty); \
    return INT2NUM(tty->name); \
} \
static VALUE rb_set_##name(VALUE self, VALUE val) \
{ \
    struct termios *tty; \
    TypedData_Get_Struct(self, struct termios, &stty_termios_type, tty); \
    tty->name = NUM2INT(val); \
    return val; \
} \

TC_FLAG_ATTR(c_iflag)
TC_FLAG_ATTR(c_oflag)
TC_FLAG_ATTR(c_cflag)
TC_FLAG_ATTR(c_lflag)

#undef TC_FLAG_ATTR

void Init_stty(void)
{
    mStty = rb_define_module("Stty");
    cSttyTermios = rb_const_get(mStty, rb_intern("Termios"));

#define NUMCONST(name) rb_define_const(mStty, #name, INT2NUM(name))

    NUMCONST(TCSANOW);
    NUMCONST(TCSADRAIN);
    NUMCONST(TCSAFLUSH);
    NUMCONST(TCSASOFT);

    /* Input modes for c_iflag */
    NUMCONST(IGNBRK);   /* ignore BREAK condition */
    NUMCONST(BRKINT);   /* map BREAK to SIGINTR */
    NUMCONST(IGNPAR);   /* ignore (discard) parity errors */
    NUMCONST(PARMRK);   /* mark parity and framing errors */
    NUMCONST(INPCK);    /* enable checking of parity errors */
    NUMCONST(ISTRIP);   /* strip 8th bit off chars */
    NUMCONST(INLCR);    /* map NL into CR */
    NUMCONST(IGNCR);    /* ignore CR */
    NUMCONST(ICRNL);    /* map CR to NL (ala CRMOD) */
    NUMCONST(IXON);     /* enable output flow control */
    NUMCONST(IXOFF);    /* enable input flow control */
    NUMCONST(IXANY);    /* any char will restart after stop */
    NUMCONST(IMAXBEL);  /* ring bell on input queue full */
#ifdef IUCLC
    NUMCONST(IUCLC);    /* translate upper case to lower case */
#endif

    /* Output modes for c_oflag */
    NUMCONST(OPOST);   /* enable following output processing */
    NUMCONST(ONLCR);   /* map NL to CR-NL (ala CRMOD) */
    NUMCONST(OXTABS);  /* expand tabs to spaces */
    NUMCONST(ONOEOT);  /* discard EOT's `^D' on output) */
    NUMCONST(OCRNL);   /* map CR to NL */
#ifdef OLCUC
    NUMCONST(OLCUC);   /* translate lower case to upper case */
#endif
    NUMCONST(ONOCR);   /* No CR output at column 0 */
    NUMCONST(ONLRET);  /* NL performs CR function */

    /* Control modes for c_cflag */
    NUMCONST(CSIZE);       /* character size mask */
    NUMCONST(CS5);         /* 5 bits (pseudo) */
    NUMCONST(CS6);         /* 6 bits */
    NUMCONST(CS7);         /* 7 bits */
    NUMCONST(CS8);         /* 8 bits */
    NUMCONST(CSTOPB);      /* send 2 stop bits */
    NUMCONST(CREAD);       /* enable receiver */
    NUMCONST(PARENB);      /* parity enable */
    NUMCONST(PARODD);      /* odd parity, else even */
    NUMCONST(HUPCL);       /* hang up on last close */
    NUMCONST(CLOCAL);      /* ignore modem status lines */
    NUMCONST(CCTS_OFLOW);  /* CTS flow control of output */
    NUMCONST(CRTSCTS);     /* same as CCTS_OFLOW */
    NUMCONST(CRTS_IFLOW);  /* RTS flow control of input */
    NUMCONST(MDMBUF);      /* flow control output via Carrier */

    /* Local modes for c_lflag */
    NUMCONST(ECHOKE);      /* visual erase for line kill */
    NUMCONST(ECHOE);       /* visually erase chars */
    NUMCONST(ECHO);        /* enable echoing */
    NUMCONST(ECHONL);      /* echo NL even if ECHO is off */
    NUMCONST(ECHOPRT);     /* visual erase mode for hardcopy */
    NUMCONST(ECHOCTL);     /* echo control chars as ^(Char) */
    NUMCONST(ISIG);        /* enable signals INTR, QUIT, [D]SUSP */
    NUMCONST(ICANON);      /* canonicalize input lines */
    NUMCONST(ALTWERASE);   /* use alternate WERASE algorithm */
    NUMCONST(IEXTEN);      /* enable DISCARD and LNEXT */
    NUMCONST(EXTPROC);     /* external processing */
    NUMCONST(TOSTOP);      /* stop background jobs from output */
    NUMCONST(FLUSHO);      /* output being flushed (state) */
    NUMCONST(NOKERNINFO);  /* no kernel output from VSTATUS */
    NUMCONST(PENDIN);      /* XXX retype pending input (state) */
    NUMCONST(NOFLSH);      /* don't flush after interrupt */

    /* speeds */
    NUMCONST(B0);
    NUMCONST(B50);
    NUMCONST(B75);
    NUMCONST(B110);
    NUMCONST(B134);
    NUMCONST(B150);
    NUMCONST(B200);
    NUMCONST(B300);
    NUMCONST(B600);
    NUMCONST(B1200);
    NUMCONST(B1800);
    NUMCONST(B2400);
    NUMCONST(B4800);
    NUMCONST(B9600);
    NUMCONST(B19200);
    NUMCONST(B38400);
#ifndef _POSIX_C_SOURCE
    NUMCONST(EXTA);
    NUMCONST(EXTB);
#endif  /*_POSIX_C_SOURCE */

    /* flush constants */
    NUMCONST(TCIFLUSH);   /* Flush data received but not read. */
    NUMCONST(TCOFLUSH);   /* Flush data written but not transmitted. */
    NUMCONST(TCIOFLUSH);  /* Flush both data received but not read and data written but not transmitted. */

    /* c_cc array */
    NUMCONST(VEOF);
    NUMCONST(VEOL);
    NUMCONST(VEOL2);
    NUMCONST(VERASE);
    NUMCONST(VWERASE);
    NUMCONST(VKILL);
    NUMCONST(VREPRINT);
    NUMCONST(VINTR);
    NUMCONST(VQUIT);
    NUMCONST(VSUSP);
    NUMCONST(VDSUSP);
    NUMCONST(VSTART);
    NUMCONST(VSTOP);
    NUMCONST(VLNEXT);
    NUMCONST(VDISCARD);
    NUMCONST(VMIN);
    NUMCONST(VTIME);
    NUMCONST(VSTATUS);
    NUMCONST(NCCS);

    rb_define_alloc_func(cSttyTermios, allocate);
    rb_define_private_method(cSttyTermios, "tcgetattr", rb_tcgetattr, 1);
    rb_define_private_method(cSttyTermios, "tcsetattr", rb_tcsetattr, 2);
    rb_define_private_method(cSttyTermios, "cfsetspeed", rb_cfsetspeed, 1);
    rb_define_private_method(cSttyTermios, "tcflush", rb_tcflush, 2);
    rb_define_private_method(cSttyTermios, "get_c_cc", rb_get_c_cc, 1);
    rb_define_private_method(cSttyTermios, "set_c_cc", rb_set_c_cc, 2);

    rb_define_method(cSttyTermios, "input_speed", rb_cfgetispeed, 0);
    rb_define_method(cSttyTermios, "output_speed", rb_cfgetispeed, 0);

#define TC_FLAG_ATTR(name) \
    rb_define_method(cSttyTermios, #name, rb_get_##name, 0); \
    rb_define_method(cSttyTermios, #name "=", rb_set_##name, 1)

    TC_FLAG_ATTR(c_iflag);
    TC_FLAG_ATTR(c_oflag);
    TC_FLAG_ATTR(c_cflag);
    TC_FLAG_ATTR(c_lflag);

#undef TC_FLAG_ATTR
}
/* vim: set noet sws=4 sw=4: */
