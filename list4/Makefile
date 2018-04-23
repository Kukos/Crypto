THIS_DIR := $(PROJECT_DIR)/list4

IDIR := $(THIS_DIR)/include
SDIR := $(THIS_DIR)/src
LDIR := $(PROJECT_DIR)/external/libs

SRCS := $(wildcard $(SDIR)/*.c)
SRCS += $(ESDIR)/log.c
SRCS += $(ESDIR)/assert.c
OBJS := $(SRCS:%.c=%.o)
DEPS := $(wildcard $(IDIR)/*.h)
DEPS += $(wildcard $(EIDIR)/*.h)

LIBS := -lgmp -lbitset -lcstring

EXEC := $(THIS_DIR)/mh_system.out

%.o: %.c
	$(call print_cc, $<)
	$(Q)$(CC) $(C_TEST_FLAGS) -I$(IDIR) -I$(EIDIR) -c $< -o $@

$(EXEC): $(OBJS)
	$(call print_bin, $@)
	$(Q)$(CC) $(C_TEST_FLAGS) -L$(LDIR) -I$(IDIR) -I$(EIDIR) $(OBJS) $(LIBS) -o $@

clean:
	$(call print_info,Cleaning)
	$(Q)$(RM) $(OBJS)
	$(Q)$(RM) $(T_OBJS)
	$(Q)$(RM) $(EXEC)
	$(Q)$(RM) $(T_EXEC)
