#ifndef LIBMFDBG_HH
#define LIBMFDBG_HH

#include <stdexcept>
#include <stdint.h>
#include <string>
#include <memory>
#include <limits>
#include <vector>
#include <set>

class Debug8052 {
public:
	typedef enum {
		ptrtype_none,
		ptrtype_function,
		ptrtype_generic,
		ptrtype_code,
		ptrtype_xram,
		ptrtype_iram,
		ptrtype_paged,
		ptrtype_upper128
	} ptrtype_t;

	typedef enum {
		scope_global = 0,
		scope_file = 1,
		scope_function = 2,
		scope_symbol = 3,
		scope_null = 4
	} scope_t;

	typedef enum {
		addrspace_xstack,
		addrspace_istack,
		addrspace_code,
		addrspace_codestatic,
		addrspace_iramlow,
		addrspace_xram,
		addrspace_iram,
		addrspace_bit,
		addrspace_sfr,
		addrspace_sbit,
		addrspace_register,
		addrspace_undefined
	} addrspace_t;

	typedef enum {
		findsl_none = 0,
		findsl_c = (1 << 0),
		findsl_asm = (1 << 1),
		findsl_delimiter = (1 << 2),
		findsl_c_asm = findsl_c | findsl_asm,
		findsl_all = findsl_c | findsl_asm | findsl_delimiter
	} findsl_flags_t;

	class Scope {
	public:
		Scope(const std::string& name = "", scope_t scope = scope_null,
		      const std::string& filename = "", const std::string& funcname = "",
		      unsigned int level = 0, unsigned int block = 0);
		int compare(const Scope& x) const;
		bool operator<(const Scope& x) const;
		const std::string& get_name(void) const { return m_name; }
		const std::string& get_funcname(void) const { return m_funcname; }
		const std::string& get_filename(void) const { return m_filename; }
		unsigned int get_level(void) const { return m_level; }
		unsigned int get_block(void) const { return m_block; }
		scope_t get_scope(void) const { return m_scope; }
		operator std::string(void) const;

		void set_name(const std::string& n) { m_name = n; }
		void set_funcname(const std::string& sn) { m_funcname = sn; }
		void set_filename(const std::string& sn) { m_filename = sn; }
		void set_level(unsigned int l) { m_level = l; }
		void set_block(unsigned int b) { m_block = b; }
		void set_scope(scope_t s) { m_scope = s; }

	protected:
		std::string m_name;
		std::string m_filename;
		std::string m_funcname;
		unsigned int m_level;
		unsigned int m_block;
		scope_t m_scope;
	};

	class SourceRange {
	public:
		class SourceLocation {
		public:
			SourceLocation(uint16_t fileid = std::numeric_limits<uint16_t>::max(), uint32_t lineno = 0, uint16_t colno = 0);
			uint16_t get_fileid(void) const { return m_fileid; }
			uint32_t get_lineno(void) const { return m_lineno; }
			uint16_t get_colno(void) const { return m_colno; }

			void set_fileid(uint16_t x) { m_fileid = x; }
			void set_lineno(uint32_t x) { m_lineno = x; }
			void set_colno(uint16_t x) { m_colno = x; }

			int compare(const SourceLocation& x) const;
			std::string to_str(const Debug8052& dbg) const;

		protected:
			uint32_t m_lineno;
			uint16_t m_fileid;
			uint16_t m_colno;
		};

		SourceRange(uint16_t fileid = std::numeric_limits<uint16_t>::max(), uint32_t lineno = 0);
		SourceRange(uint16_t sfileid, uint32_t slineno, uint16_t scolno, uint16_t efileid, uint32_t elineno, uint16_t ecolno);
		const SourceLocation& get_loc(unsigned int idx) const { return m_loc[!!idx]; }
		SourceLocation& get_loc(unsigned int idx) { return m_loc[!!idx]; }
		const SourceLocation& get_start(void) const { return m_loc[0]; }
		SourceLocation& get_start(void) { return m_loc[0]; }
		const SourceLocation& get_end(void) const { return m_loc[1]; }
		SourceLocation& get_end(void) { return m_loc[1]; }
		uint16_t get_fileid(unsigned int idx) const { return get_loc(idx).get_fileid(); }
		uint32_t get_lineno(unsigned int idx) const { return get_loc(idx).get_lineno(); }
		uint16_t get_colno(unsigned int idx) const { return get_loc(idx).get_colno(); }
		uint16_t get_start_fileid(void) const { return get_start().get_fileid(); }
		uint32_t get_start_lineno(void) const { return get_start().get_lineno(); }
		uint16_t get_start_colno(void) const { return get_start().get_colno(); }
		uint16_t get_end_fileid(void) const { return get_end().get_fileid(); }
		uint32_t get_end_lineno(void) const { return get_end().get_lineno(); }
		uint16_t get_end_colno(void) const { return get_end().get_colno(); }

		bool is_singleline(void) const;
		bool is_singleline_nocol(void) const;
		bool is_valid(unsigned int idx) const { return get_fileid(idx) != std::numeric_limits<uint16_t>::max(); }
		bool is_valid(void) const { return is_valid(0) && is_valid(1); }
		bool make_union(SourceRange& ext) const;
		bool is_overlap(const SourceRange& x) const { SourceRange ext(x); return make_union(ext); }
		int compare(const SourceRange& x) const;
		bool operator==(const SourceRange& x) const { return compare(x) == 0; }
		bool operator!=(const SourceRange& x) const { return compare(x) != 0; }
		bool operator<(const SourceRange& x) const { return compare(x) < 0; }
		bool operator<=(const SourceRange& x) const { return compare(x) <= 0; }
		bool operator>(const SourceRange& x) const { return compare(x) > 0; }
		bool operator>=(const SourceRange& x) const { return compare(x) >= 0; }
		std::string to_str(const Debug8052& dbg) const;

	protected:
		SourceLocation m_loc[2];
	};

	class TypeChain {
	public:
		typedef std::shared_ptr<TypeChain> typechainptr_t;
		typedef std::shared_ptr<const TypeChain> const_typechainptr_t;

		class EnumValue {
		public:
			EnumValue(uint32_t val, const std::string& name);
			uint32_t get_value(void) const { return m_value; }
			const std::string& get_name(void) const { return m_name; }
			int compare(const EnumValue& x) const;
			bool operator==(const EnumValue& x) const { return compare(x) == 0; }
			bool operator!=(const EnumValue& x) const { return compare(x) != 0; }
			bool operator<(const EnumValue& x) const { return compare(x) < 0; }
			bool operator<=(const EnumValue& x) const { return compare(x) <= 0; }
			bool operator>(const EnumValue& x) const { return compare(x) > 0; }
			bool operator>=(const EnumValue& x) const { return compare(x) >= 0; }

		protected:
			std::string m_name;
			uint32_t m_value;
		};

		virtual typechainptr_t clone(void) const = 0;
		virtual bool is_void(void) const { return false; }
		virtual bool is_simpledatatype(void) const { return false; }
		virtual bool is_signed(void) const { return false; }
		virtual bool is_float(void) const { return false; }
		virtual bool is_bit(void) const { return false; }
		virtual bool is_bitfield(void) const { return false; }
		virtual unsigned int get_bitfieldoffset(void) const { return 0; }
		virtual unsigned int get_bitfieldlength(void) const { return 0; }
		virtual bool is_struct(void) const { return false; }
		virtual unsigned int get_structnrfields(void) const { return 0; }
		virtual unsigned int get_structfieldoffset(unsigned int i) const { return 0; }
		virtual const std::string& get_structfieldname(unsigned int i) const { return nullstr; }
		virtual const typechainptr_t get_structfieldtype(unsigned int i) const { return typechainptr_t(); }
		virtual bool is_enum(void) const { return false; }
		virtual const std::string& get_enumname(void) const { return nullstr; }
		virtual const EnumValue *get_enumbyvalue(uint32_t val) const { return 0; }
		virtual const EnumValue *get_enumbyname(const std::string& name) const { return 0; }
		virtual const EnumValue *get_firstenum(void) const { return 0; }
		virtual const EnumValue *get_nextenum(const EnumValue *x) const { return 0; }
		virtual unsigned int get_size(void) const { return 0; }
		virtual bool is_pointer(void) const { return get_pointertype() != ptrtype_none; }
		virtual ptrtype_t get_pointertype(void) const { return ptrtype_none; }
		virtual bool is_arraydim(void) const { return false; }
		virtual unsigned int get_arraydim(void) const { return 0; }
		virtual const typechainptr_t get_subtype(void) const { return typechainptr_t(); }
		virtual std::string get_typestr(void) const = 0;
		virtual std::ostream& print(std::ostream& os, unsigned int indent) const = 0;
		virtual bool is_equivalent(const_typechainptr_t tc) const = 0;

	protected:
		static const std::string nullstr;
	};

	class Symbol {
	public:
		class Location {
		public:
			Location(void);
			addrspace_t get_addrspace(void) const { return m_addrspace; }
			bool is_onstack(void) const { return m_onstack; }
			int16_t get_stackoffset(void) const { return m_stackoffset; }
			unsigned int get_nrregisters(void) const { return m_registers.size(); }
			const std::string& get_register(unsigned int i) const { return m_registers[i]; }
			int32_t get_addr(void) const { return m_addr; }
			int32_t get_endaddr(void) const { return m_endaddr; }
			bool is_addr_valid(void) const { return m_addr != -1; }
			bool is_endaddr_valid(void) const { return m_endaddr != -1; }

			bool operator==(const Location& x) const;
			bool operator!=(const Location& x) const;

			void set_addrspace(addrspace_t as) { m_addrspace = as; }
			void set_onstack(bool onstack) { m_onstack = onstack; }
			void set_stackoffset(int16_t so) { m_stackoffset = so; }
			void clear_registers(void) { m_registers.clear(); }
			void add_register(const std::string& r) { m_registers.push_back(r); }
			void set_addr(void) { m_addr = -1; }
			void set_endaddr(void) { m_endaddr = -1; }
			void set_addr(int32_t a) { m_addr = a; }
			void set_endaddr(int32_t ea) { m_endaddr = ea; }

		protected:
			typedef std::vector<std::string> registers_t;
			registers_t m_registers;
			int32_t m_addr;
			int32_t m_endaddr;
			int16_t m_stackoffset;
			addrspace_t m_addrspace;
			bool m_onstack;
		};

		class Liverange : public Location {
		public:
			Liverange(uint32_t pc = ~0, uint32_t endpc = ~0);
			Liverange(uint32_t pc, uint32_t endpc, const Location& loc);
			uint32_t get_pc(void) const { return m_pc; }
			uint32_t get_endpc(void) const { return m_endpc; }
			void set_pc(uint32_t pc = ~0) { m_pc = pc; }
			void set_endpc(uint32_t endpc = ~0) { m_endpc = endpc; }
			int compare(const Liverange& x) const;
			bool operator<(const Liverange& x) const;
			bool is_overlap(const Liverange& x) const;
			bool is_overlap(uint32_t pc, uint32_t endpc) const;
			bool is_adjacent(const Liverange& x) const;
			bool is_adjacent(uint32_t pc, uint32_t endpc) const;
			bool is_inside(uint32_t pc) const;
			bool is_inside(const Liverange& x) const;
			bool is_inside(uint32_t pc, uint32_t endpc) const;

		protected:
			uint32_t m_pc;
			uint32_t m_endpc;
		};

		Symbol(const Scope& scope = Scope(), bool isfunc = false);

		Scope& get_scope(void) { return m_scope; }
		const Scope& get_scope(void) const { return m_scope; }
		const TypeChain::typechainptr_t get_type(void) const { return m_type; }
		bool is_function(void) const { return m_isfunction; }
		bool is_interrupt(void) const { return m_isinterrupt; }
		uint8_t get_interruptnr(void) const { return m_interruptnr; }
		uint8_t get_interruptregbank(void) const { return m_interruptregbank; }

		Location& get_location(void);
		const Location& get_location(void) const;
		Location& get_location(uint32_t pc);
		const Location& get_location(uint32_t pc) const;
		bool is_valid(uint32_t pc) const;
		bool has_liveranges(void) const;
		void add_liverange(const Liverange& x);
		void clear_liverange(uint32_t pc, uint32_t endpc);
		const Liverange& get_liverange(uint32_t pc) const;

		int compare(const Symbol& x) const;
		bool operator<(const Symbol& x) const;

		void set_scope(const Scope& s) { m_scope = s; }
		void set_type(TypeChain::typechainptr_t t) { m_type = t; }
		void set_function(bool func) { m_isfunction = func; }
		void set_interrupt(bool irq) { m_isinterrupt = irq; }
		void set_interruptnr(uint8_t irqnr) { m_interruptnr = irqnr; }
		void set_interruptregbank(uint8_t irqregs) { m_interruptregbank = irqregs; }

		void set_location(const Location& loc) { m_location = loc; }

		std::string get_liveranges_str(void) const;
		bool check_liveranges_canonical(void) const;

	protected:
		Scope m_scope;
		Location m_location;
		TypeChain::typechainptr_t m_type;
		typedef std::set<Liverange> liveranges_t;
		liveranges_t m_liveranges;
		uint8_t m_interruptnr;
		uint8_t m_interruptregbank;
		bool m_isfunction;
		bool m_isinterrupt;
	};

	class Record {
	public:
		Record(const std::string& name = "", const std::string& filename = "", TypeChain::typechainptr_t t = TypeChain::typechainptr_t());
		const std::string& get_name(void) const { return m_name; }
		const std::string& get_filename(void) const { return m_filename; }
		int compare(const Record& x) const;
		bool operator<(const Record& x) const;
		
		const TypeChain::typechainptr_t get_type(void) const { return m_type; }
		TypeChain::typechainptr_t get_type(void) { return m_type; }

		void set_name(const std::string& n) { m_name = n; }
		void set_filename(const std::string& n) { m_filename = n; }
		void set_type(TypeChain::typechainptr_t t) { m_type = t; }

	protected:
		std::string m_name;
		std::string m_filename;
		TypeChain::typechainptr_t m_type; 
	};

	class SourceLine {
	public:
		class Reachable {
		public:
			typedef enum {
				kind_statement,
				kind_call,
				kind_return
			} kind_t;

			Reachable(uint32_t addr = 0, kind_t kind = kind_statement);
			uint32_t get_addr(void) const { return m_addr; }
			kind_t get_kind(void) const { return m_kind; }
			int compare(const Reachable& x) const;
			bool operator<(const Reachable& x) const;

		protected:
			uint32_t m_addr;
			kind_t m_kind;
		};

		class ReachableSet : public std::set<Reachable> {
		public:
			bool is_return(void) const;
			bool is_reachable(uint32_t addr) const;
		};

		typedef enum {
			kind_c,
			kind_asm,
			kind_delimiter
		} kind_t;

		SourceLine(const SourceRange& sr = SourceRange(), unsigned int level = 0, unsigned int block = 0,
			   uint32_t addr = 0, kind_t kind = kind_c);

		int compare(const SourceLine& x) const;
		bool operator<(const SourceLine& x) const;

		const SourceRange& get_srcrange(void) const { return m_srcrange; }
		SourceRange& get_srcrange(void) { return m_srcrange; }
		void set_srcrange(const SourceRange& sr) { m_srcrange = sr; }
		void add_srcrange(const SourceRange& sr);

		typedef std::set<SourceRange> addnsrcranges_t;
		const addnsrcranges_t& get_addnsrcranges(void) const { return m_addnsrcranges; }
		addnsrcranges_t& get_addnsrcranges(void) { return m_addnsrcranges; }
		void set_addnsrcranges(const addnsrcranges_t& sr) { m_addnsrcranges = sr; }

	        const ReachableSet& get_reachable(void) const { return m_reachable; }
		ReachableSet& get_reachable(void) { return m_reachable; }
		void set_reachable(const ReachableSet& sr) { m_reachable = sr; }

		unsigned int get_level(void) const { return m_level; }
		unsigned int get_block(void) const { return m_block; }
		uint32_t get_addr(void) const { return m_addr; }
		kind_t get_kind(void) const { return m_kind; }
		findsl_flags_t get_findslflag(void) const;

		void set_level(unsigned int l) { m_level = l; }
		void set_block(unsigned int b) { m_block = b; }
		void set_addr(uint16_t a) { m_addr = a; }
		void set_kind(kind_t k) { m_kind = k; }

	protected:
		ReachableSet m_reachable;
		SourceRange m_srcrange;
		addnsrcranges_t m_addnsrcranges;
		unsigned int m_level;
		unsigned int m_block;
		uint32_t m_addr;
		kind_t m_kind;
	};

	class CFI {
	public:
		static addrspace_t to_addrspace(uint8_t seg);

		class ExprEval {
		public:
			typedef int32_t exprtype_t;
			typedef uint32_t exprutype_t;

			virtual uint16_t read_pc(void) const = 0;
			virtual void write_pc(uint16_t val) = 0;
			virtual exprtype_t read_cfa(unsigned int idx) const = 0;
			virtual void write_cfa(unsigned int idx, exprtype_t val) = 0;
			virtual void write_cfa(unsigned int idx) = 0;
			virtual exprtype_t read_physical_reg(const std::string& name, unsigned int sz) const = 0;
			virtual void write_physical_reg(const std::string& name, unsigned int sz, exprtype_t val) = 0;
			virtual exprtype_t read_virtual_reg(const std::string& name, unsigned int sz) const = 0;
			virtual void write_virtual_reg(const std::string& name, unsigned int sz, exprtype_t val) = 0;
			virtual exprtype_t read_mem(addrspace_t as, unsigned int sz, exprtype_t offs) const = 0;
			virtual void write_mem(addrspace_t as, unsigned int sz, exprtype_t offs, exprtype_t val) = 0;
			virtual exprtype_t read_overlay(const std::string& name, unsigned int sz, exprtype_t offs) const = 0;
			virtual void write_overlay(const std::string& name, unsigned int sz, exprtype_t offs, exprtype_t val) = 0;
		};

		class Resources;

		class Resource : public std::vector<uint16_t> {
		public:
			typedef enum {
				kind_physical,
				kind_virtual,
				kind_framecell,
				kind_invalid
			} kind_t;

			Resource(kind_t kind = kind_invalid, const std::string& name = "", unsigned int sz = 0);
			Resource(uint16_t baseframe, int32_t offs, unsigned int sz);

			const std::string& get_name(void) const { return m_name; }
			void set_name(const std::string& n) { m_name = n; }
			unsigned int get_size(void) const { return m_size; }
			void set_size(unsigned int sz) { m_size = sz; }
			ExprEval::exprtype_t get_sizemask(void) const { return (1ULL << get_size()) - 1; }
			uint16_t get_baseframe(void) const;
			void set_baseframe(uint16_t baseframe);
			int32_t get_frameoffset(void) const;
			void set_frameoffset(int32_t offs);
			kind_t get_kind(void) const { return m_kind; }
			void set_kind(kind_t k);
			std::string to_str(const std::shared_ptr<const Debug8052::CFI::Resources>& rsrc =
					   std::shared_ptr<const Debug8052::CFI::Resources>()) const;
			ExprEval::exprtype_t read(const ExprEval& eval, const std::shared_ptr<const Debug8052::CFI::Resources>& rsrc) const;
			void write(ExprEval& eval, const std::shared_ptr<const Debug8052::CFI::Resources>& rsrc, ExprEval::exprtype_t val) const;

		protected:
			std::string m_name;
			uint16_t m_size;
			kind_t m_kind;
		};

		class Frame {
		public:
			typedef enum {
				kind_stack,
				kind_static_overlay,
				kind_base_address,
				kind_invalid
			} kind_t;

			Frame(void);
			Frame(uint16_t spreg, addrspace_t spseg);
			Frame(addrspace_t baseseg);
			Frame(const std::string& segname);

			kind_t get_kind(void) const { return m_kind; }
			uint16_t get_spreg(void) const;
			addrspace_t get_spseg(void) const;
			addrspace_t get_baseseg(void) const;
			const std::string& get_segname(void) const;

			std::string to_str(const std::shared_ptr<const Debug8052::CFI::Resources>& rsrc =
					   std::shared_ptr<const Debug8052::CFI::Resources>()) const;
			ExprEval::exprtype_t read(ExprEval& eval, const std::shared_ptr<const Debug8052::CFI::Resources>& rsrc,
						  unsigned int sz, ExprEval::exprtype_t offset) const;
			void write(ExprEval& eval, const std::shared_ptr<const Debug8052::CFI::Resources>& rsrc,
				   unsigned int sz, ExprEval::exprtype_t offset, ExprEval::exprtype_t val) const;

		protected:
			kind_t m_kind;
			std::string m_param;
		};

		class Resources : public std::vector<Resource> {
		protected:
			typedef std::vector<Resource> base_t;

		public:
			typedef std::shared_ptr<Resources> ptr_t;
			typedef std::shared_ptr<const Resources> const_ptr_t;
			static const Resource invalid_resource;
			static const Frame invalid_frame;

			Resource& operator[](size_type i);
			const Resource& operator[](size_type i) const;
			Frame& frame(size_type i);
			const Frame& frame(size_type i) const;
			size_type frame_size(void) const { return m_frames.size(); }

			std::string to_str(size_type i) const;
			static std::string to_str(const const_ptr_t& r, size_type i);

		protected:
			typedef std::vector<Frame> frames_t;
			frames_t m_frames;
		};

		class Common {
		public:
			typedef std::shared_ptr<Common> ptr_t;
			typedef std::shared_ptr<const Common> const_ptr_t;

			Common(const Resources::const_ptr_t& cfir = Resources::const_ptr_t(),
			       uint16_t retaddr = 0, addrspace_t retaddrseg = addrspace_undefined);
			const Resources::const_ptr_t& get_cfiresources(void) const { return m_cfir; }
			void set_cfiresources(const Resources::const_ptr_t& cfir = Resources::const_ptr_t()) { m_cfir = cfir; }
			uint16_t get_retaddr(void) const { return m_retaddr; }
			void set_retaddr(uint16_t ra) { m_retaddr = ra; }
			addrspace_t get_retaddrseg(void) const { return m_retaddrseg; }
			void set_retaddrseg(addrspace_t ras) { m_retaddrseg = ras; }
			ExprEval::exprtype_t evaluate_retaddr(const ExprEval& eval) const;

		protected:
			Resources::const_ptr_t m_cfir;
			uint16_t m_retaddr;
			addrspace_t m_retaddrseg;
		};

		class Expression : public std::vector<uint8_t> {
		protected:
			typedef std::vector<uint8_t> base_t;

		public:
			Expression(const uint8_t *instr = 0, unsigned int instrsz = 0);
			Expression(unsigned int x, int32_t offs = 0, bool cfa = false);

			typedef enum {
				mode_expression,
				mode_resource,
				mode_cfa,
				mode_constant,
				mode_undefined,
				mode_samevalue
			} mode_t;

			mode_t get_mode(void) const { return m_mode; }
			bool is_expression(void) const { return get_mode() == mode_expression; }
			bool is_resource(void) const { return get_mode() == mode_resource; }
			bool is_cfa(void) const { return get_mode() == mode_cfa; }
			bool is_constant(void) const { return get_mode() == mode_constant; }
			bool is_undefined(void) const { return get_mode() == mode_undefined; }
			bool is_samevalue(void) const { return get_mode() == mode_samevalue; }

			void set_undefined(void);
			void set_samevalue(void);
			void set_constant(int32_t v = 0);

			unsigned int get_resource(void) const;
			int32_t get_offset(void) const;

			std::string to_str(const Resources::const_ptr_t& r = Resources::const_ptr_t()) const;
			ExprEval::exprtype_t evaluate(ExprEval& eval, const std::shared_ptr<const Debug8052::CFI::Resources>& rsrc, unsigned int sz) const;

			int compare(const Expression& x) const;
			bool operator==(const Expression& x) const { return compare(x) == 0; }
			bool operator!=(const Expression& x) const { return compare(x) != 0; }
			bool operator<(const Expression& x) const { return compare(x) < 0; }
			bool operator<=(const Expression& x) const { return compare(x) <= 0; }
			bool operator>(const Expression& x) const { return compare(x) > 0; }
			bool operator>=(const Expression& x) const { return compare(x) >= 0; }

		protected:
			mode_t m_mode;

			uint8_t loadbinu8(size_type& ptr) const;
			uint16_t loadbinu16(size_type& ptr) const;
			uint32_t loadbinu32(size_type& ptr) const;
			uint64_t loadbinuleb(size_type& ptr) const;
			int64_t loadbinsleb(size_type& ptr) const;
		};

		class TableExprVector : public std::vector<Expression> {
		protected:
			typedef std::vector<Expression> base_t;
			static const Expression invalid_expr;

		public:
			Expression& operator[](size_type i);
			const Expression& operator[](size_type i) const;

			int compare_expr(const TableExprVector& x) const;
		};

		class TableEntry : public TableExprVector {
		protected:
			typedef TableExprVector base_t;

		public:
			typedef enum {
				kind_picker,
				kind_conditional,
				kind_simple,
				kind_invalid,
				kind_delimiter
			} kind_t;

			TableEntry(const Common::const_ptr_t& cfic = Common::const_ptr_t(), uint32_t addr = 0, kind_t kind = kind_simple, uint32_t condaddr = 0);
			const Common::const_ptr_t& get_cficommon(void) const { return m_cficommon; }
			void set_cficommon(const Common::const_ptr_t& cfic = Common::const_ptr_t()) { m_cficommon = cfic; }
			const Resources::const_ptr_t& get_cfiresources(void) const { return m_cficommon->get_cfiresources(); }
			uint32_t get_addr(void) const { return m_addr; }
			void set_addr(uint32_t addr) { m_addr = addr; }
			void add_addr(uint32_t addr) { m_addr += addr; }

			uint32_t get_condaddr(void) const { return m_condaddr; }
			void set_condaddr(uint32_t addr) { m_condaddr = addr; }

			kind_t get_kind(void) const { return m_kind; }
			void set_kind(kind_t k) { m_kind = k; }

			base_t& cfa(void) { return m_cfa; }
			const base_t& cfa(void) const { return m_cfa; }

			std::string to_str(void) const;
			void unwind(ExprEval& eval) const;
			ExprEval::exprtype_t evaluate_retaddr(const ExprEval& eval) const;

			int compare(const TableEntry& x) const;
			bool operator==(const TableEntry& x) const { return compare(x) == 0; }
			bool operator!=(const TableEntry& x) const { return compare(x) != 0; }
			bool operator<(const TableEntry& x) const { return compare(x) < 0; }
			bool operator<=(const TableEntry& x) const { return compare(x) <= 0; }
			bool operator>(const TableEntry& x) const { return compare(x) > 0; }
			bool operator>=(const TableEntry& x) const { return compare(x) >= 0; }

		protected:
			Common::const_ptr_t m_cficommon;
			base_t m_cfa;
			uint32_t m_addr;
			uint32_t m_condaddr;
			kind_t m_kind;
		};
	};

	// Debug Info Lookup
	virtual const Debug8052::Record *dbg_find_record(const std::string& name, const std::string& filename) const = 0;
	virtual const Debug8052::Record *dbg_find_first_record(void) const = 0;
	virtual const Debug8052::Record *dbg_find_next_record(const Debug8052::Record *rec) const = 0;
	virtual const Debug8052::Symbol *dbg_find_symbol(Debug8052::addrspace_t as, uint16_t addr) const = 0;
	virtual const Debug8052::Symbol *dbg_find_first_symbol(void) const = 0;
	virtual const Debug8052::Symbol *dbg_find_next_symbol(const Debug8052::Symbol *sym) const = 0;
	virtual const Debug8052::Symbol *dbg_find_namespace_symbol(const std::string& name, bool pc_valid = false, uint32_t pc = 0) const = 0;
	virtual const Debug8052::SourceLine *dbg_find_sourceline(uint32_t addr, findsl_flags_t flags = findsl_c_asm) const = 0;
	virtual const Debug8052::SourceLine *dbg_find_next_sourceline(const Debug8052::SourceLine *sl, findsl_flags_t flags = findsl_c_asm) const = 0;
	typedef std::set<std::string> modules_t;
	virtual modules_t dbg_find_modules(bool asmb = false) const = 0;
	virtual const std::string& dbg_find_filename(uint16_t fileid) const = 0;
};

inline Debug8052::findsl_flags_t operator|(Debug8052::findsl_flags_t a, Debug8052::findsl_flags_t b)
{
	return (Debug8052::findsl_flags_t)((unsigned int)a | (unsigned int)b);
}

inline Debug8052::findsl_flags_t operator&(Debug8052::findsl_flags_t a, Debug8052::findsl_flags_t b)
{
	return (Debug8052::findsl_flags_t)((unsigned int)a & (unsigned int)b);
}

inline Debug8052::findsl_flags_t operator~(Debug8052::findsl_flags_t a)
{
	return (Debug8052::findsl_flags_t)(~(unsigned int)a);
}

inline Debug8052::findsl_flags_t& operator|=(Debug8052::findsl_flags_t& a, Debug8052::findsl_flags_t b)
{
	return a = a | b;
}

inline Debug8052::findsl_flags_t& operator&=(Debug8052::findsl_flags_t& a, Debug8052::findsl_flags_t b)
{
	return a = a & b;
}

class MicrofootDebugger {
public:
	virtual ~MicrofootDebugger();

	// Routines for finding and opening interfaces
	// (Factory)
	typedef std::string serial_t;
	typedef std::set<serial_t> serials_t;

	static serials_t find_interfaces(void);
	static MicrofootDebugger *open_interface(const serial_t& serial, bool enable_debuglink = true);
	static MicrofootDebugger *open_simulator(void);

	// Query properties
	virtual const serial_t& get_interface_serial(void) const = 0;

	// Read Binary File
	typedef enum {
		filetype_omf51,
		filetype_ihex,
		filetype_cdb,
		filetype_ubrof,
		filetype_unknown
	} filetype_t;
	typedef enum {
		fileload_none = 0,
		fileload_code = (1 << 0),
		fileload_debug = (1 << 1),
		fileload_code_debug = fileload_code | fileload_debug
	} fileload_t;
	static filetype_t determine_filetype(std::istream& is);
	static filetype_t determine_filetype(const std::string& filename);
	virtual void load_file(std::istream& is, filetype_t ft = filetype_unknown,
			       fileload_t flags = fileload_code_debug, uint16_t fill_bkpt_top_addr = 0) = 0;
	virtual void load_file(const std::string& filename, filetype_t ft = filetype_unknown,
			       fileload_t flags = fileload_code_debug, uint16_t fill_bkpt_top_addr = 0) = 0;

	// Address Spaces
	typedef enum { addrspace_code, addrspace_direct, addrspace_indirect, addrspace_x, addrspace_flash, addrspace_sfr, addrspace_p } addrspace_t;
	static addrspace_t to_addrspace(Debug8052::addrspace_t as);
	static Debug8052::addrspace_t to_debugaddrspace(addrspace_t as);
	virtual uint8_t read_mem(addrspace_t as, uint16_t addr) = 0;
	virtual void write_mem(addrspace_t as, uint16_t addr, uint8_t data) = 0;
	virtual uint16_t read_pc(void) = 0;
	virtual void write_pc(uint16_t pc) = 0;

	// CPU State
	typedef enum {
		cpustate_disconnected,
		cpustate_locked,
		cpustate_halt,
		cpustate_run,
		cpustate_busy
	} cpustate_t;

	typedef uint64_t key_t;
	typedef std::vector<key_t> keylist_t;
	typedef std::set<uint16_t> codeaddrset_t;

	virtual cpustate_t cpu_getstate(void) = 0;
	virtual int cpu_getbusypercentage(void) { return -1; }
	virtual key_t cpu_connect(const keylist_t& keys = keylist_t()) = 0;
	key_t cpu_connect(key_t key);
	virtual void cpu_disconnect(void) = 0;
	virtual void cpu_hwreset(bool on) = 0;

	virtual void cpu_run(const codeaddrset_t& tempbkpaddr = codeaddrset_t()) = 0;
	virtual void cpu_stop(void) = 0;
	virtual void cpu_reset(void) = 0;
	virtual void cpu_step(void) = 0;
	virtual void cpu_stepline(void) = 0;
	virtual void cpu_stepinto(void) = 0;
	virtual void cpu_stepout(void) = 0;
	virtual void cpu_writeback(void) = 0;
	virtual void cpu_bulkerase(bool keepcalib = true, const keylist_t& oldkeys = keylist_t()) = 0;
	void cpu_bulkerase(bool keepcalib, key_t oldkey);
	virtual void cpu_writekey(key_t key = ~0ULL) = 0;
	virtual void cpu_writeprotect(uint16_t addr) = 0;
	virtual void cpu_eraseprotect(uint16_t addr) = 0;

	// Breakpoints
	class Breakpoint {
	public:
		Breakpoint(uint16_t addr = 0, bool ena = false, unsigned int count = 0, void *userptr = 0)
			: m_userptr(userptr), m_count(count), m_addr(addr), m_enabled(ena), m_fired(false) {}
	        bool is_enabled(void) const { return m_enabled; }
		bool has_fired(void) const { return m_fired; }
		uint16_t get_addr(void) const { return m_addr; }
		unsigned int get_count(void) const { return m_count; }
		void *get_userptr(void) const { return m_userptr; }
		void enable(bool ena = true) { m_enabled = ena; }
		void disable(void) { enable(false); }
		void set_addr(uint16_t addr) { m_addr = addr; }
		void set_count(unsigned int count = 0) { m_count = count; }
		void set_userptr(void *userptr = 0) { m_userptr = userptr; }
		void set_fired(bool f = true)  { m_fired = f; }
		void reset_fired(void) { m_fired = false; }

	protected:
		void *m_userptr;
		unsigned int m_count;
		uint16_t m_addr;
		bool m_enabled;
		bool m_fired;
	};

	virtual unsigned int breakpoint_count(void) const = 0;
	virtual const Breakpoint *breakpoint_get(unsigned int nr) const = 0;
	virtual Breakpoint *breakpoint_get(unsigned int nr) = 0;
	virtual unsigned int breakpoint_new(void) = 0;
	virtual void breakpoint_delete(unsigned int nr) = 0;

	// Debug Link
	virtual bool debuglink_txfull(void) = 0;
	virtual bool debuglink_rxempty(void) = 0;
	virtual unsigned int debuglink_txcount(void) = 0;
	virtual unsigned int debuglink_txfree(void) = 0;
	virtual unsigned int debuglink_rxcount(void) = 0;
	virtual unsigned int debuglink_rxfree(void) = 0;
	virtual void debuglink_txwrite(uint8_t v) = 0;
	virtual uint8_t debuglink_rxread(void) = 0;

	// Pin Emulation
	typedef enum {
		pinemulation_none   = 0,
		pinemulation_portb6 = (1 << 0),
		pinemulation_portb7 = (1 << 1),
		pinemulation_dirb6  = (1 << 2),
		pinemulation_dirb7  = (1 << 3),
		pinemulation_drvb6  = (1 << 4),
		pinemulation_drvb7  = (1 << 5),
		pinemulation_enable = (1 << 6)
	} pinemulation_t;
	virtual pinemulation_t pinemulation_get(void) = 0;
	virtual void pinemulation_set(pinemulation_t pinemul) = 0;

	// Backtrace
	class BacktraceFrame {
	public:
		BacktraceFrame(uint16_t addr, uint8_t sp)
			: m_addr(addr), m_sp(sp) {}

		uint16_t get_addr(void) const { return m_addr; }
		uint8_t get_sp(void) const { return m_sp; }

	protected:
		uint16_t m_addr;
		uint8_t m_sp;
	};

	typedef std::vector<BacktraceFrame> backtrace_t;

	virtual backtrace_t backtrace_get(void) = 0;

	// CPU trace
	class CPUTrace {
	public:
		typedef enum {
			type_disconnected = cpustate_disconnected,
			type_locked = cpustate_locked,
			type_halt = cpustate_halt,
			type_run = cpustate_run,
			type_singlestep,
			type_breakpoint,
			type_flashbulkerase,
			type_flashwrite,
			type_dbglinkwrite,
			type_dbglinkread,
			type_truncate
		} type_t;

		CPUTrace(type_t typ = type_disconnected, uint16_t addr = 0, long seconds = 0, long microseconds = 0);
		long get_seconds(void) const { return m_seconds; }
		long get_microseconds(void) const { return m_microseconds; }
		uint16_t get_addr(void) const { return m_addr; }
		type_t get_type(void) const { return m_type; }
		static bool has_addr(type_t t);
		bool has_addr(void) const { return has_addr(get_type()); }
		static bool has_char(type_t t);
		bool has_char(void) const { return has_char(get_type()); }

	protected:
		long m_seconds;
		long m_microseconds;
		uint16_t m_addr;
		type_t m_type;
	};

	typedef std::vector<CPUTrace> cputrace_t;
	virtual cputrace_t cputrace_get(void) = 0;
	virtual unsigned int cputrace_get_length(void) const = 0;
	virtual void cputrace_set_length(unsigned int len) = 0;

	// Profile
	class CPUProfileLocation {
	public:
		CPUProfileLocation(const Debug8052::SourceLine& sl = Debug8052::SourceLine(), unsigned int hitcnt = 0)
			: m_sourceline(sl), m_hitcount(hitcnt) {}
		const Debug8052::SourceLine& get_sourceline(void) const { return m_sourceline; }
		unsigned int get_hitcount(void) const { return m_hitcount; }
		bool operator<(const CPUProfileLocation& x) const { return get_sourceline() < x.get_sourceline(); }
		void increment_hitcount(void) { ++m_hitcount; }

	protected:
		Debug8052::SourceLine m_sourceline;
		unsigned int m_hitcount;
	};

	class CPUProfile : public std::set<CPUProfileLocation> {
	public:
		unsigned int total_hitcount(void) const;
	};

	virtual CPUProfile profile_get(void) = 0;
	virtual Debug8052::findsl_flags_t  profile_get_enable(void) const = 0;
	virtual void profile_set_enable(Debug8052::findsl_flags_t flags = Debug8052::findsl_none) = 0;

	// Tracing
	typedef enum {
		trace_off,
		trace_error,
		trace_normal,
		trace_lowlevelio,
		trace_poll
	} trace_t;

	virtual void trace(trace_t t = trace_off) = 0;
	virtual void trace(trace_t t, std::ostream& os) = 0;
	virtual bool is_trace(trace_t flg) const = 0;
	virtual void trace_out(const std::string& s) = 0;

	virtual void test(void) {}
	virtual void dumpmem(uint8_t codecfg = 0) {}

	// Debug Info Lookup
	virtual const Debug8052::Record *dbg_find_record(const std::string& name, const std::string& filename) const = 0;
	virtual const Debug8052::Symbol *dbg_find_symbol(Debug8052::addrspace_t as, uint16_t addr) const = 0;
	virtual const Debug8052::Symbol *dbg_find_first_symbol(void) const = 0;
	virtual const Debug8052::Symbol *dbg_find_next_symbol(const Debug8052::Symbol *sym) const = 0;
	virtual const Debug8052::Symbol *dbg_find_symbol(const std::string& name) const = 0;
	virtual const Debug8052::Symbol *dbg_find_symbol(const std::string& name, const std::string& filename) const = 0;
	virtual const Debug8052::Symbol *dbg_find_namespace_symbol(const std::string& name, bool pc_valid = false, uint32_t pc = 0) const = 0;
	virtual const Debug8052::SourceLine *dbg_find_sourceline(uint32_t addr, Debug8052::findsl_flags_t flags = Debug8052::findsl_c_asm) const = 0;
	virtual const Debug8052::SourceLine *dbg_find_next_sourceline(const Debug8052::SourceLine *sl, Debug8052::findsl_flags_t flags = Debug8052::findsl_c_asm) const = 0;
	virtual const Debug8052::SourceLine *dbg_find_sourceline(const std::string& name, unsigned int line, uint16_t col, bool exact = false, Debug8052::findsl_flags_t flags = Debug8052::findsl_c_asm) const = 0;
	virtual const Debug8052::SourceLine *dbg_find_sourceline(const std::string& name, unsigned int line, unsigned int level, unsigned int block, Debug8052::findsl_flags_t flags) const = 0;
	virtual Debug8052::modules_t dbg_find_modules(bool asmb = false) const = 0;
	virtual const std::string& dbg_find_filename(uint16_t fileid) const = 0;
	virtual const Debug8052::CFI::TableEntry *dbg_find_first_cfi(void) const = 0;
	virtual const Debug8052::CFI::TableEntry *dbg_find_next_cfi(const Debug8052::CFI::TableEntry *cfite) const = 0;
	virtual const Debug8052::CFI::TableEntry *dbg_find_cfi(uint32_t addr) const = 0;

	typedef enum {
		compilervendor_sdcc,
		compilervendor_keil,
		compilervendor_iar,
		compilervendor_wickenhaeuser,
		compilervendor_noice
	} compilervendor_t;
	virtual compilervendor_t get_compilervendor(void) const = 0;
	virtual void set_compilervendor(compilervendor_t cv) = 0;
	virtual Debug8052::ptrtype_t addrtag_to_ptrtype(uint8_t tag) = 0;
	virtual uint8_t ptrtype_to_addrtag(Debug8052::ptrtype_t ptype) = 0;
	virtual addrspace_t addrtag_to_addrspace(uint8_t tag) = 0;
	virtual uint8_t addrspace_to_addrtag(addrspace_t as) = 0;
	virtual bool is_compiler_msbfirst(void) = 0;

	// Expression Evaluator
	class Register;

	class ExprValue {
	public:
		typedef enum {
			exprtype_lvalue,
			exprtype_regs,
			exprtype_int,
			exprtype_float,
			exprtype_string
		} exprtype_t;

		class Register {
		public:
			Register(Debug8052::addrspace_t as = Debug8052::addrspace_undefined, uint16_t addr = 0) : m_addr(addr), m_as(as) {}
			Debug8052::addrspace_t get_addrspace(void) const { return m_as; }
			uint16_t get_addr(void) const { return m_addr; }

		protected:
			uint16_t m_addr;
			Debug8052::addrspace_t m_as;
		};

		ExprValue(const ExprValue& e);
		ExprValue(Debug8052::TypeChain::typechainptr_t typ, Debug8052::addrspace_t as, uint16_t addr);
		ExprValue(Debug8052::TypeChain::typechainptr_t typ, const std::vector<Register>& r);
		ExprValue(long val);
		ExprValue(double val);
		ExprValue(const std::string& val);
		ExprValue(const char *val);
		ExprValue();
		~ExprValue();
		ExprValue& operator=(const ExprValue& e);
		void set_value(Debug8052::TypeChain::typechainptr_t typ, Debug8052::addrspace_t as, uint16_t addr);
		void set_value(Debug8052::TypeChain::typechainptr_t typ, const std::vector<Register>& r);
		void set_value(long val);
		void set_value(double val);
		void set_value(const std::string& val);
		void set_value(const char *val);
		exprtype_t get_exprtype(void) const { return m_exprtype; }
		Debug8052::addrspace_t get_addrspace(void) const;
		uint16_t get_addr(void) const;
		unsigned int get_nrregisters(void) const;
		const Register *get_register(unsigned int idx) const;
		Debug8052::TypeChain::typechainptr_t get_type(void) const;
		Debug8052::TypeChain::typechainptr_t get_type(void);
		long get_intvalue(void) const;
		double get_floatvalue(void) const;
		std::string get_stringvalue(void) const;

	protected:
		exprtype_t m_exprtype;
		Debug8052::TypeChain::typechainptr_t m_type;
		union {
			struct {
				uint16_t addr;
				Debug8052::addrspace_t as;
				uint8_t flags;
			} lvalue;

			struct {
				unsigned int nrregs;
				Register *regs;
			} regs;

			long intvalue;

			double floatvalue;

			struct {
				unsigned int length;
				char *data;
			} stringvalue;
		} m_val;

		static Debug8052::TypeChain::typechainptr_t default_type(exprtype_t exprtype, unsigned int len);
		Debug8052::TypeChain::typechainptr_t default_type(void) const { return default_type(m_exprtype, m_val.stringvalue.length); }
		void destroy(void);
		void postcopy(void);
	};
	virtual Debug8052::TypeChain::typechainptr_t parse_ctype(const std::string& s) = 0;
	virtual ExprValue parse_cexpr(const std::string& s, bool loadvalue = false) = 0;
	virtual ExprValue parse_cexpr(const std::string& s, bool loadvalue, uint32_t pc) = 0;

	// Register Description
	class Register {
	public:
		Register(const std::string& regname = "", const std::string& periphname = "", const std::string& description = "",
			 uint16_t addr = 0, Debug8052::addrspace_t as = Debug8052::addrspace_sfr, uint8_t readmask = 0, uint8_t writemask = 0, uint8_t dflt = 0,
			 bool readsafe = true, bool mappedtox = true);
		const std::string& get_regname(void) const { return m_regname; }
		const std::string& get_periphname(void) const { return m_periphname; }
		const std::string& get_description(void) const { return m_description; }
		uint16_t get_addr(void) const { return m_addr; }
		Debug8052::addrspace_t get_addrspace(void) const { return m_addrspace; }
		uint8_t get_readmask(void) const { return m_readmask; }
		uint8_t get_writemask(void) const { return m_writemask; }
		uint8_t get_default(void) const { return m_default; }
		bool is_readsafe(void) const { return m_readsafe; }
		bool is_mappedtox(void) const { return m_mappedtox; }
		int compare(const Register& x) const;
		bool operator<(const Register& x) const;

	protected:
		std::string m_regname;
		std::string m_periphname;
		std::string m_description;
		uint16_t m_addr;
		Debug8052::addrspace_t m_addrspace;
		uint8_t m_readmask;
		uint8_t m_writemask;
		uint8_t m_default;
		bool m_readsafe;
		bool m_mappedtox;
	};

	// Register List Lookup
	virtual const Register *reg_find(Debug8052::addrspace_t as, uint16_t addr) = 0;
	virtual const Register *reg_find_first(void) = 0;
	virtual const Register *reg_find_next(const Register *reg) = 0;

	typedef std::string chipid_t;
	typedef std::set<chipid_t> chipids_t;
	virtual chipids_t chips_current(void) const = 0;
	virtual chipids_t chips_available(void) const = 0;
	virtual void chips_autodetect(void) = 0;
	virtual void chips_set(const chipids_t& ids = chipids_t()) = 0;
	virtual void chips_read_file(const std::string& fn) = 0;
	virtual void chips_read_dir(const std::string& dir) = 0;
	virtual void chips_write_dir(const std::string& dir) = 0;
	virtual void chips_clear(void) = 0;

	// Simulator
	typedef uint64_t simtime_t;
	static const simtime_t simtime_end;;
	typedef enum {
		simgpio_0 = 0,
		simgpio_1 = 1,
		simgpio_l = 2,
		simgpio_h = 3,
		simgpio_z = 4,
		simgpio_x = 5
	} simgpio_t;

	virtual bool is_simulator(void) const { return false; }
	virtual simtime_t sim_gettime(void) const { return 0; }
	virtual void sim_stopat(simtime_t time = 0);
	virtual void sim_reset(void);
	virtual unsigned int sim_nrgpio(void) const { return 0; }
	virtual simgpio_t sim_getgpio(unsigned int nr) const;
	virtual void sim_setgpio(unsigned int nr, simgpio_t val);
	virtual std::string sim_gpioname(unsigned int nr);

	// Disassembly
	static unsigned int insn_length(uint8_t opcode);
	static std::string insn_disass(uint32_t code, uint16_t addr = 0);
	static std::string insn_disass_sym(uint32_t code, uint16_t addr, const Debug8052& dbginfo);
	virtual std::string insn_disass_sym(uint32_t code, uint16_t addr = 0) = 0;

	typedef enum {
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// ARITHMETIC OPERATIONS
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Add Register To Accumulator
		opcode_ADD_A_R0               = 0x28,
		opcode_ADD_A_R1               = 0x29,
		opcode_ADD_A_R2               = 0x2A,
		opcode_ADD_A_R3               = 0x2B,
		opcode_ADD_A_R4               = 0x2C,
		opcode_ADD_A_R5               = 0x2D,
		opcode_ADD_A_R6               = 0x2E,
		opcode_ADD_A_R7               = 0x2F,
		// Add Direct Byte To Accumulator
		opcode_ADD_A_D                = 0x25,
		// Add Indirect Ram To Accumulator
		opcode_ADD_A_ATR0             = 0x26,
		opcode_ADD_A_ATR1             = 0x27,
		// Add Immediate Data To Accumulator
		opcode_ADD_A_DATA             = 0x24,
		// Add Register To Accumulator With Carry
		opcode_ADDC_A_R0              = 0x38,
		opcode_ADDC_A_R1              = 0x39,
		opcode_ADDC_A_R2              = 0x3A,
		opcode_ADDC_A_R3              = 0x3B,
		opcode_ADDC_A_R4              = 0x3C,
		opcode_ADDC_A_R5              = 0x3D,
		opcode_ADDC_A_R6              = 0x3E,
		opcode_ADDC_A_R7              = 0x3F,
		// Add Direct Byte To Accumulator With Carry
		opcode_ADDC_A_D               = 0x35,
		// Add Indirect Ram To Accumulator With Carry
		opcode_ADDC_A_ATR0            = 0x36,
		opcode_ADDC_A_ATR1            = 0x37,
		// Add Immediate Data To Accumulator With Carry
		opcode_ADDC_A_DATA            = 0x34,
		// Subtract Register From Accumulator With Borrow
		opcode_SUBB_A_R0              = 0x98,
		opcode_SUBB_A_R1              = 0x99,
		opcode_SUBB_A_R2              = 0x9A,
		opcode_SUBB_A_R3              = 0x9B,
		opcode_SUBB_A_R4              = 0x9C,
		opcode_SUBB_A_R5              = 0x9D,
		opcode_SUBB_A_R6              = 0x9E,
		opcode_SUBB_A_R7              = 0x9F,
		// Subtract Direct Byte From Accumulator With Borrow
		opcode_SUBB_A_D               = 0x95,
		// Subtract Indirect Ram From Accumulator With Borrow
		opcode_SUBB_A_ATR0            = 0x96,
		opcode_SUBB_A_ATR1            = 0x97,
		// Subtract Immediate Data From Accumulator With Borrow
		opcode_SUBB_A_DATA            = 0x94,
		// Increment Accumulator
		opcode_INC_A                  = 0x04,
		// Increment Register
		opcode_INC_R0                 = 0x08,
		opcode_INC_R1                 = 0x09,
		opcode_INC_R2                 = 0x0A,
		opcode_INC_R3                 = 0x0B,
		opcode_INC_R4                 = 0x0C,
		opcode_INC_R5                 = 0x0D,
		opcode_INC_R6                 = 0x0E,
		opcode_INC_R7                 = 0x0F,
		// Increment Direct Byte
		opcode_INC_D                  = 0x05,
		// Increment Indirect Ram
		opcode_INC_ATR0               = 0x06,
		opcode_INC_ATR1               = 0x07,
		// Increment Data Pointer
		opcode_INC_DPTR               = 0xA3,
		// Decrement Accumulator
		opcode_DEC_A                  = 0x14,
		// Decrement Register
		opcode_DEC_R0                 = 0x18,
		opcode_DEC_R1                 = 0x19,
		opcode_DEC_R2                 = 0x1A,
		opcode_DEC_R3                 = 0x1B,
		opcode_DEC_R4                 = 0x1C,
		opcode_DEC_R5                 = 0x1D,
		opcode_DEC_R6                 = 0x1E,
		opcode_DEC_R7                 = 0x1F,
		// Decrement Direct Byte
		opcode_DEC_D                  = 0x15,
		// Decrement Indirect Ram
		opcode_DEC_ATR0               = 0x16,
		opcode_DEC_ATR1               = 0x17,
		// Multiply A And B
		opcode_MUL_AB                 = 0xA4,
		// Divide A By B
		opcode_DIV_AB                 = 0x84,
		// Decimal Adjust Accumulator
		opcode_DA_A                   = 0xD4,
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// LOGICAL OPERATION
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// AND Register To Accumulator
		opcode_ANL_A_R0               = 0x58,
		opcode_ANL_A_R1               = 0x59,
		opcode_ANL_A_R2               = 0x5A,
		opcode_ANL_A_R3               = 0x5B,
		opcode_ANL_A_R4               = 0x5C,
		opcode_ANL_A_R5               = 0x5D,
		opcode_ANL_A_R6               = 0x5E,
		opcode_ANL_A_R7               = 0x5F,
		// AND Direct Byte To Accumulator
		opcode_ANL_A_D                = 0x55,
		// AND Indirect Ram To Accumulator
		opcode_ANL_A_ATR0             = 0x56,
		opcode_ANL_A_ATR1             = 0x57,
		// AND Immediate Data To Accumulator
		opcode_ANL_A_DATA             = 0x54,
		// AND Accumulator To Direct Byte
		opcode_ANL_D_A                = 0x52,
		// AND Immediate Data To Direct Byte
		opcode_ANL_D_DATA             = 0x53,
		// OR Register To Accumulator
		opcode_ORL_A_R0               = 0x48,
		opcode_ORL_A_R1               = 0x49,
		opcode_ORL_A_R2               = 0x4A,
		opcode_ORL_A_R3               = 0x4B,
		opcode_ORL_A_R4               = 0x4C,
		opcode_ORL_A_R5               = 0x4D,
		opcode_ORL_A_R6               = 0x4E,
		opcode_ORL_A_R7               = 0x4F,
		// OR Direct Byte To Accumulator
		opcode_ORL_A_D                = 0x45,
		// OR Indirect Ram To Accumulator
		opcode_ORL_A_ATR0             = 0x46,
		opcode_ORL_A_ATR1             = 0x47,
		// OR Immediate Data To Accumulator
		opcode_ORL_A_DATA             = 0x44,
		// OR Accumulator To Direct Byte
		opcode_ORL_D_A                = 0x42,
		// OR Immediate Data To Direct Byte
		opcode_ORL_D_DATA             = 0x43,
		// Exclusive-OR Register To Accumulator
		opcode_XRL_A_R0               = 0x68,
		opcode_XRL_A_R1               = 0x69,
		opcode_XRL_A_R2               = 0x6A,
		opcode_XRL_A_R3               = 0x6B,
		opcode_XRL_A_R4               = 0x6C,
		opcode_XRL_A_R5               = 0x6D,
		opcode_XRL_A_R6               = 0x6E,
		opcode_XRL_A_R7               = 0x6F,
		// Exclusive-OR Direct Byte To Accumulator
		opcode_XRL_A_D                = 0x65,
		// Exclusive-OR Indirect Ram To Accumulator
		opcode_XRL_A_ATR0             = 0x66,
		opcode_XRL_A_ATR1             = 0x67,
		// Exclusive-OR Immediate Data To Accumulator
		opcode_XRL_A_DATA             = 0x64,
		// Exclusive-OR Accumulator To Direct Byte
		opcode_XRL_D_A                = 0x62,
		// Exclusive-OR Immediate Data To Direct Byte
		opcode_XRL_D_DATA             = 0x63,
		// Clear Accumulator
		opcode_CLR_A                  = 0xE4,
		// Complement Accumulator
		opcode_CPL_A                  = 0xF4,
		// Rotate Accumulator Left
		opcode_RL_A                   = 0x23,
		// Rotate Accumulator Left Through The Carry
		opcode_RLC_A                  = 0x33,
		// Rotate Accumulator Right
		opcode_RR_A                   = 0x03,
		// Rotate Accumulator Right Through The Carry
		opcode_RRC_A                  = 0x13,
		// Swap Nibbles Within The Accumulator
		opcode_SWAP_A                 = 0xC4,
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// DATA TRANSFER
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Move Register To Accumulator
		opcode_MOV_A_R0               = 0xE8,
		opcode_MOV_A_R1               = 0xE9,
		opcode_MOV_A_R2               = 0xEA,
		opcode_MOV_A_R3               = 0xEB,
		opcode_MOV_A_R4               = 0xEC,
		opcode_MOV_A_R5               = 0xED,
		opcode_MOV_A_R6               = 0xEE,
		opcode_MOV_A_R7               = 0xEF,
		// Move Direct Byte To Accumulator
		opcode_MOV_A_D                = 0xE5,
		// Move Indirect Ram To Accumulator
		opcode_MOV_A_ATR0             = 0xE6,
		opcode_MOV_A_ATR1             = 0xE7,
		// Move Immediate Data To Accumulator
		opcode_MOV_A_DATA             = 0x74,
		// Move Accumulator To Register
		opcode_MOV_R0_A               = 0xF8,
		opcode_MOV_R1_A               = 0xF9,
		opcode_MOV_R2_A               = 0xFA,
		opcode_MOV_R3_A               = 0xFB,
		opcode_MOV_R4_A               = 0xFC,
		opcode_MOV_R5_A               = 0xFD,
		opcode_MOV_R6_A               = 0xFE,
		opcode_MOV_R7_A               = 0xFF,
		// Move Direct Byte To Register
		opcode_MOV_R0_D               = 0xA8,
		opcode_MOV_R1_D               = 0xA9,
		opcode_MOV_R2_D               = 0xAA,
		opcode_MOV_R3_D               = 0xAB,
		opcode_MOV_R4_D               = 0xAC,
		opcode_MOV_R5_D               = 0xAD,
		opcode_MOV_R6_D               = 0xAE,
		opcode_MOV_R7_D               = 0xAF,
		// Move Immediate Data To Register
		opcode_MOV_R0_DATA            = 0x78,
		opcode_MOV_R1_DATA            = 0x79,
		opcode_MOV_R2_DATA            = 0x7A,
		opcode_MOV_R3_DATA            = 0x7B,
		opcode_MOV_R4_DATA            = 0x7C,
		opcode_MOV_R5_DATA            = 0x7D,
		opcode_MOV_R6_DATA            = 0x7E,
		opcode_MOV_R7_DATA            = 0x7F,
		// Move Accumulator To Direct Byte
		opcode_MOV_D_A                = 0xF5,
		// Move Register To Direct Byte
		opcode_MOV_D_R0               = 0x88,
		opcode_MOV_D_R1               = 0x89,
		opcode_MOV_D_R2               = 0x8A,
		opcode_MOV_D_R3               = 0x8B,
		opcode_MOV_D_R4               = 0x8C,
		opcode_MOV_D_R5               = 0x8D,
		opcode_MOV_D_R6               = 0x8E,
		opcode_MOV_D_R7               = 0x8F,
		// Move Direct Byte To Direct
		opcode_MOV_D_D                = 0x85,
		// Move Indirect Ram To Direct Byte
		opcode_MOV_D_ATR0             = 0x86,
		opcode_MOV_D_ATR1             = 0x87,
		// Move Immediate Data To Direct Byte
		opcode_MOV_D_DATA             = 0x75,
		// Move Accumulator To Indirect Ram
		opcode_MOV_ATR0_A             = 0xF6,
		opcode_MOV_ATR1_A             = 0xF7,
		// Move Direct Byte To Indirect Ram
		opcode_MOV_ATR0_D             = 0xA6,
		opcode_MOV_ATR1_D             = 0xA7,
		// Move Immediate Data To Indirect Ram
		opcode_MOV_ATR0_DATA          = 0x76,
		opcode_MOV_ATR1_DATA          = 0x77,
		// Load Data Pointer With a 16-Bit Constant
		opcode_MOV_DPTR_DATA          = 0x90,
		// Move Code Byte Relative To DPTR To Accumulator
		opcode_MOVC_A_ATDPTR          = 0x93,
		// Move Code Byte Relative To PC To Accumulator
		opcode_MOVC_A_ATPC            = 0x83,
		// Move Accumulator To Program Memory
		opcode_MOVC_ADPTR_A           = 0xA5,
		// Move External Ram (8-Bit Address) To Accumulator
		opcode_MOVX_A_ATR0            = 0xE2,
		opcode_MOVX_A_ATR1            = 0xE3,
		// Move External Ram (16-Bit Address) To Accumulator
		opcode_MOVX_A_ATDPTR          = 0xE0,
		// Move Accumulator To External Ram (8-Bit Address)
		opcode_MOVX_ATR0_A            = 0xF2,
		opcode_MOVX_ATR1_A            = 0xF3,
		// Move Accumulator To External Ram (16-Bit Address)
		opcode_MOVX_ATDPTR_A          = 0xF0,
		// Push Direct Byte Onto Stack
		opcode_PUSH                   = 0xC0,
		// Pop Direct Byte From Stack
		opcode_POP                    = 0xD0,
		// Exchange Register With Accumulator
		opcode_XCH_A_R0               = 0xC8,
		opcode_XCH_A_R1               = 0xC9,
		opcode_XCH_A_R2               = 0xCA,
		opcode_XCH_A_R3               = 0xCB,
		opcode_XCH_A_R4               = 0xCC,
		opcode_XCH_A_R5               = 0xCD,
		opcode_XCH_A_R6               = 0xCE,
		opcode_XCH_A_R7               = 0xCF,
		// Exchange Direct Byte With Accumulator
		opcode_XCH_A_D                = 0xC5,
		// Exchange Indirect Ram With Accumulator
		opcode_XCH_A_ATR0             = 0xC6,
		opcode_XCH_A_ATR1             = 0xC7,
		// Exchange Low-Order Digit Indirect Ram With Accumulator
		opcode_XCHD_A_ATR0            = 0xD6,
		opcode_XCHD_A_ATR1            = 0xD7,
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// BOOLEAN VARIABLE MANIPULATION
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Clear Carry
		opcode_CLR_C                  = 0xC3,
		// Clear Direct Bit
		opcode_CLR_BIT                = 0xC2,
		// Set Carry
		opcode_SETB_C                 = 0xD3,
		// Set Direct Bit
		opcode_SETB_BIT               = 0xD2,
		// Complement Carry
		opcode_CPL_C                  = 0xB3,
		// Complement Direct Bit
		opcode_CPL_BIT                = 0xB2,
		// AND Direct Bit To Carry
		opcode_ANL_C_BIT              = 0x82,
		// AND Complement Of Direct Bit To Carry
		opcode_ANL_C_NBIT             = 0xB0,
		// OR Direct Bit To Carry
		opcode_ORL_C_BIT              = 0x72,
		// OR Complement Of Direct Bit To Carry
		opcode_ORL_C_NBIT             = 0xA0,
		// Move Direct Bit To Carry
		opcode_MOV_C_BIT              = 0xA2,
		// Move Carry To Direct Bit
		opcode_MOV_BIT_C              = 0x92,
		// Jump If Carry Is Set
		opcode_JC                     = 0x40,
		// Jump If Carry Is Not Set
		opcode_JNC                    = 0x50,
		// Jump If Direct Bit Is Set
		opcode_JB                     = 0x20,
		// Jump If Direct Bit Is Not Set
		opcode_JNB                    = 0x30,
		// Jump If Direct Bit Is Set And Clear Bit
		opcode_JBC                    = 0x10,
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// PROGRAM BRANCHING
		// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// Absolute Subroutine Call
		opcode_ACALL_PG0              = 0x11,
		opcode_ACALL_PG1              = 0x31,
		opcode_ACALL_PG2              = 0x51,
		opcode_ACALL_PG3              = 0x71,
		opcode_ACALL_PG4              = 0x91,
		opcode_ACALL_PG5              = 0xB1,
		opcode_ACALL_PG6              = 0xD1,
		opcode_ACALL_PG7              = 0xF1,
		// Long Subroutine Call
		opcode_LCALL                  = 0x12,
		// Return From Subroutine
		opcode_RET                    = 0x22,
		// Return From Interrupt
		opcode_RETI                   = 0x32,
		// Absolute Jump
		opcode_AJMP_PG0               = 0x01,
		opcode_AJMP_PG1               = 0x21,
		opcode_AJMP_PG2               = 0x41,
		opcode_AJMP_PG3               = 0x61,
		opcode_AJMP_PG4               = 0x81,
		opcode_AJMP_PG5               = 0xA1,
		opcode_AJMP_PG6               = 0xC1,
		opcode_AJMP_PG7               = 0xE1,
		// Long Jump
		opcode_LJMP                   = 0x02,
		// Short Jump (Relative Address)
		opcode_SJMP                   = 0x80,
		// Jump Indirect Relative To The DPTR
		opcode_JMP_A_DPTR             = 0x73,
		// Jump If Accumulator Is Zero
		opcode_JZ                     = 0x60,
		// Jump If Accumulator Is Not Zero
		opcode_JNZ                    = 0x70,
		// Compare Direct Byte To Accumulator And Jump If Not equal
		opcode_CJNE_A_D               = 0xB5,
		// Compare Immediate To Accumulator And Jump If Not equal
		opcode_CJNE_A_DATA            = 0xB4,
		// Compare Immediate To Register And Jump If Not equal
		opcode_CJNE_R0_DATA           = 0xB8,
		opcode_CJNE_R1_DATA           = 0xB9,
		opcode_CJNE_R2_DATA           = 0xBA,
		opcode_CJNE_R3_DATA           = 0xBB,
		opcode_CJNE_R4_DATA           = 0xBC,
		opcode_CJNE_R5_DATA           = 0xBD,
		opcode_CJNE_R6_DATA           = 0xBE,
		opcode_CJNE_R7_DATA           = 0xBF,
		// Compare Immediate To Indirect And Jump If Not Equal
		opcode_CJNE_ATR0_DATA         = 0xB6,
		opcode_CJNE_ATR1_DATA         = 0xB7,
		// Decrement Register And jump If Not Zero
		opcode_DJNZ_R0                = 0xD8,
		opcode_DJNZ_R1                = 0xD9,
		opcode_DJNZ_R2                = 0xDA,
		opcode_DJNZ_R3                = 0xDB,
		opcode_DJNZ_R4                = 0xDC,
		opcode_DJNZ_R5                = 0xDD,
		opcode_DJNZ_R6                = 0xDE,
		opcode_DJNZ_R7                = 0xDF,
		// Decrement Direct Byte And jump If Not Zero
		opcode_DJNZ_D                 = 0xD5,
		// No Operation
		opcode_NOP                    = 0x00
	} opcode_t;

	// Utility Functions
	static uint8_t crc8_ccitt_byte(uint8_t crc, uint8_t c);
	static uint8_t crc8_ccitt(const uint8_t *buf, uint8_t len, uint8_t init);

protected:
	class OpcodeTable;
	static const OpcodeTable opcode_table[256];

	static uint8_t crc8_ccitt_reduce(uint8_t x);
};

const std::string& to_str(Debug8052::ptrtype_t pt);
inline std::ostream& operator<<(std::ostream& os, Debug8052::ptrtype_t pt) { return os << to_str(pt); }
const std::string& to_str(Debug8052::scope_t sc);
inline std::ostream& operator<<(std::ostream& os, Debug8052::scope_t sc) { return os << to_str(sc); }
const std::string& to_str(Debug8052::addrspace_t as);
inline std::ostream& operator<<(std::ostream& os, Debug8052::addrspace_t as) { return os << to_str(as); }
std::ostream& operator<<(std::ostream& os, const Debug8052::Scope& sc);
const std::string& to_str(Debug8052::SourceLine::kind_t k);
inline std::ostream& operator<<(std::ostream& os, Debug8052::SourceLine::kind_t k) { return os << to_str(k); }
const std::string& to_str(Debug8052::SourceLine::Reachable::kind_t k);
inline std::ostream& operator<<(std::ostream& os, Debug8052::SourceLine::Reachable::kind_t k) { return os << to_str(k); }
const std::string& to_str(Debug8052::CFI::Resource::kind_t k);
inline std::ostream& operator<<(std::ostream& os, Debug8052::CFI::Resource::kind_t k) { return os << to_str(k); }
const std::string& to_str(Debug8052::CFI::Frame::kind_t k);
inline std::ostream& operator<<(std::ostream& os, Debug8052::CFI::Frame::kind_t k) { return os << to_str(k); }
const std::string& to_str(Debug8052::CFI::Expression::mode_t m);
inline std::ostream& operator<<(std::ostream& os, Debug8052::CFI::Expression::mode_t m) { return os << to_str(m); }
const std::string& to_str(Debug8052::CFI::TableEntry::kind_t k);
inline std::ostream& operator<<(std::ostream& os, Debug8052::CFI::TableEntry::kind_t k) { return os << to_str(k); }
const std::string& to_str(MicrofootDebugger::cpustate_t cs);
inline std::ostream& operator<<(std::ostream& os, MicrofootDebugger::cpustate_t cs) { return os << to_str(cs); }
const std::string& to_str(MicrofootDebugger::addrspace_t as);
inline std::ostream& operator<<(std::ostream& os, MicrofootDebugger::addrspace_t as) { return os << to_str(as); }
const std::string& to_str(MicrofootDebugger::filetype_t ft);
inline std::ostream& operator<<(std::ostream& os, MicrofootDebugger::filetype_t ft) { return os << to_str(ft); }
const std::string& to_str(MicrofootDebugger::fileload_t fl);
inline std::ostream& operator<<(std::ostream& os, MicrofootDebugger::fileload_t fl) { return os << to_str(fl); }
const std::string& to_str(MicrofootDebugger::compilervendor_t cv);
inline std::ostream& operator<<(std::ostream& os, MicrofootDebugger::compilervendor_t cv) { return os << to_str(cv); }
const std::string& to_str(MicrofootDebugger::trace_t t);
inline std::ostream& operator<<(std::ostream& os, MicrofootDebugger::trace_t t) { return os << to_str(t); }
std::string to_str(MicrofootDebugger::pinemulation_t fl);
inline std::ostream& operator<<(std::ostream& os, MicrofootDebugger::pinemulation_t fl) { return os << to_str(fl); }
std::string to_str(MicrofootDebugger::CPUTrace::type_t t);
inline std::ostream& operator<<(std::ostream& os, MicrofootDebugger::CPUTrace::type_t t) { return os << to_str(t); }
std::string to_str(MicrofootDebugger::ExprValue::exprtype_t t);
inline std::ostream& operator<<(std::ostream& os, MicrofootDebugger::ExprValue::exprtype_t t) { return os << to_str(t); }

inline MicrofootDebugger::fileload_t operator|(MicrofootDebugger::fileload_t a, MicrofootDebugger::fileload_t b)
{
	return (MicrofootDebugger::fileload_t)((unsigned int)a | (unsigned int)b);
}

inline MicrofootDebugger::fileload_t operator&(MicrofootDebugger::fileload_t a, MicrofootDebugger::fileload_t b)
{
	return (MicrofootDebugger::fileload_t)((unsigned int)a & (unsigned int)b);
}

inline MicrofootDebugger::fileload_t operator~(MicrofootDebugger::fileload_t a)
{
	return (MicrofootDebugger::fileload_t)(~(unsigned int)a);
}

inline MicrofootDebugger::fileload_t& operator|=(MicrofootDebugger::fileload_t& a, MicrofootDebugger::fileload_t b)
{
	return a = a | b;
}

inline MicrofootDebugger::fileload_t& operator&=(MicrofootDebugger::fileload_t& a, MicrofootDebugger::fileload_t b)
{
	return a = a & b;
}

inline MicrofootDebugger::pinemulation_t operator|(MicrofootDebugger::pinemulation_t a, MicrofootDebugger::pinemulation_t b)
{
	return (MicrofootDebugger::pinemulation_t)((unsigned int)a | (unsigned int)b);
}

inline MicrofootDebugger::pinemulation_t operator&(MicrofootDebugger::pinemulation_t a, MicrofootDebugger::pinemulation_t b)
{
	return (MicrofootDebugger::pinemulation_t)((unsigned int)a & (unsigned int)b);
}

inline MicrofootDebugger::pinemulation_t operator~(MicrofootDebugger::pinemulation_t a)
{
	return (MicrofootDebugger::pinemulation_t)(~(unsigned int)a);
}

inline MicrofootDebugger::pinemulation_t& operator|=(MicrofootDebugger::pinemulation_t& a, MicrofootDebugger::pinemulation_t b)
{
	return a = a | b;
}

inline MicrofootDebugger::pinemulation_t& operator&=(MicrofootDebugger::pinemulation_t& a, MicrofootDebugger::pinemulation_t b)
{
	return a = a & b;
}

#endif /* LIBMFDBG_HH */
