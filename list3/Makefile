JAVAC := javac

THIS_DIR := $(PROJECT_DIR)/list3

IDIR := $(THIS_DIR)/include
SDIR := $(THIS_DIR)/src
LDIR := $(PROJECT_DIR)/external/libs

AES_SRCS := $(wildcard $(SDIR)/aes/*.c)
AES_SRCS += $(ESDIR)/log.c
AES_OBJS := $(AES_SRCS:%.c=%.o)
AES_TOBJS := $(AES_SRCS:%.c=%test.o)
AES_DEPS := $(wildcard $(IDIR)/aes/*.h)
AES_DEPS += $(wildcard $(EIDIR)/*.h)

KEYTOOL_HELPER_SDIR := $(THIS_DIR)/keytool_helper
KEYTOOL_HELPER_SRCS := $(wildcard $(KEYTOOL_HELPER_SDIR)/*.java)

LIBS := -lgetch -lcrypto -lfilebuffer

CFLAGS += -D_GNU_SOURCE

AES_EXEC := $(THIS_DIR)/encryptor.out
AES_TEST := $(THIS_DIR)/aes_test.out
CPA := $(THIS_DIR)/cpa.out

all: $(AES_EXEC) $(AES_TEST) $(CPA)

%.o: %.c
	$(call print_cc, $<)
	$(Q)$(CC) $(CFLAGS) -I$(IDIR)/aes -I$(EIDIR) -c $< -o $@

%test.o: %.c
	$(call print_cc, $<)
	$(Q)$(CC) $(C_TEST_FLAGS) -I$(IDIR)/aes -I$(EIDIR) -c $< -o $@

keytool_helper:
	$(call print_bin, $@)
	$(Q)$(JAVAC) $(KEYTOOL_HELPER_SRCS)

$(AES_EXEC): $(SDIR)/encryptor.o $(AES_OBJS) keytool_helper
	$(call print_bin, $@)
	$(Q)$(CC) $(CFLAGS) -L$(LDIR) -I$(IDIR)/aes -I$(EIDIR) $(AES_OBJS) $(SDIR)/encryptor.o $(LIBS) -o $@

$(CPA): $(SDIR)/cpa.o $(AES_OBJS)
	$(call print_bin, $@)
	$(Q)$(CC) $(CFLAGS) -L$(LDIR) -I$(IDIR)/aes -I$(EIDIR) $(AES_OBJS) $(SDIR)/cpa.o $(LIBS) -o $@

$(AES_TEST): $(SDIR)/testtest.o $(AES_TOBJS)
	$(call print_bin, $@)
	$(Q)$(CC) $(C_TEST_FLAGS) -L$(LDIR) -I$(IDIR)/aes -I$(EIDIR) $(AES_TOBJS) $(SDIR)/testtest.o $(LIBS) -o $@

clean:
	$(call print_info,Cleaning)
	$(Q)$(RM) $(AES_OBJS)
	$(Q)$(RM) $(AES_TOBJS)
	$(Q)$(RM) $(SDIR)/*.o
	$(Q)$(RM) $(THIS_DIR)/*.out
	$(Q)$(RM) $(KEYTOOL_HELPER_SDIR)/*.class
