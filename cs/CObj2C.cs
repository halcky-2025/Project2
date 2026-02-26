using SQLitePCL;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Runtime.Intrinsics;
using System.Security.Principal;
using System.Text;
using System.Threading.Tasks;

namespace Cyclon
{
    partial class Comment
    {
        public override Obj exeC(Local local)
        {
            var block = children[0] as Block;
            local.blocks.Add(block);
            block.exeC(local);
            var comelet = (CommentLet)letter;
            comelet.comment = this;
            local.comments.Add(comelet);
            comelet.Renew();
            comelet.nums.Add(0);
            local.blocks.RemoveAt(local.blocks.Count - 1);
            local.comments.RemoveAt(local.comments.Count - 1);
            return this;
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            var blk = children[0] as Block;
            local.blocks.Add(blk);
            var obj = new Vari("%GCObjectPtr", "%obj");
            var rn = new Vari("%RootNodeType*", "%rn");
            var parent = new Vari("%ElementType*", "%parent");
            var parentte = new Vari("%TreeElementType*", "%parentte");
            var deletes = new Vari("%ListType*", "%deletes");
            var n2 = LLVM.n++;
            this.model = "%com" + n2;
            var func = new Func(local.llvm, new Vari("void", "@el" + n2), rn, obj, parent, parentte, deletes);
            local.llvm.comps.Add(func);
            local.llvm.funcs.Add(func);
            var thgc = new Vari("%ThreadGCType*", "%thgc");
            local.llvm.func.comps.Add(new Load(thgc, new Vari("%ThreadGCType**", "@thgcp")));
            local.llvm.tagparents.Add(parent);
            local.llvm.tagparenttes.Add(parentte);
            var comelet = (CommentLet)letter;
            comelet.comment = this;
            comelet.Renew();
            comelet.nums.Add(0);
            local.comments.Add(comelet);
            var typedec = new TypeDec(this.model);
            local.llvm.types.Add(typedec);
            typedec.comps.Add(new TypeVal("i8*", "blk"));
            
            var checkname = "@elCheck" + n2;
            var thgc3 = new Vari("%ThreadGCType*", "%thgc");
            var ac_val = new Vari("%GC_AddClassType", "%addclass");
            var countv = new Vari("i32", "0");
            var name = "comment" + n2;
            var strv = new StrV("@" + name, name, name.Length * 1);
            local.llvm.strs.Add(strv);
            var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
            var tnp = new Vari("i32", "%tnp" + LLVM.n++);
            local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
            var tv = new Vari("i32", "%cv" + LLVM.n++);
            local.llvm.main.comps.Add(new Add(tv, tnp, typ));
            var ac_call = new Call(null, ac_val, thgc3, tv, countv, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
            local.llvm.main.comps.Add(ac_call);

            var thgc2 = new Vari("%ThreadGCType*", "%thgc");
            var i8p = new Vari("i8*", "%self");
            var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
            local.llvm.comps.Add(funccheck);
            var vari = new Vari("i8*", "%v" + LLVM.n++);
            var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
            funccheck.comps.Add(gete);
            var co_val = new Vari("%CopyObjectType", "%co");
            var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
            funccheck.comps.Add(co_load);
            var vari2 = new Vari("i8*", "%v" + LLVM.n++);
            var co_lod = new Load(vari2, vari);
            funccheck.comps.Add(co_lod);
            var co_cval = new Vari("i8*", "%v" + LLVM.n++);
            var co_call = new Call(co_cval, co_val, thgc2, vari);
            funccheck.comps.Add(co_call);
            var store = new Store(vari, co_cval);
            funccheck.comps.Add(store);

            var rn6 = new Vari("%RootNodeType*", "%rn");
            var block = new Vari("i8*", "%block");
            var decname = "@eldec" + n2;
            var funcdec = new Func(local.llvm, new Vari(model + "*", decname), rn6, block);
            local.llvm.comps.Add(funcdec);
            var thgc6 = new Vari("%ThreadGCType*", "%thgc");
            funcdec.comps.Add(new Load(thgc6, new Vari("%ThreadGCType**", "@thgcp")));

            var gmvari = new Vari("%GC_mallocType", "%gm");
            var gmload = new Load(gmvari, new Vari("%GC_mallocType*", "@GC_malloc"));
            funcdec.comps.Add(gmload);

            var go_v = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
            var tmp = new Vari("i32", this.n.ToString());
            var tmi = new Vari("i32", "%tmi" + LLVM.n++);
            funcdec.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
            var tmv = new Vari("i32", "%tmv" + LLVM.n++);
            funcdec.comps.Add(new Add(tmv, tmi, tmp));
            var go_call = new Call(go_v, gmvari, thgc6, tmv);
            funcdec.comps.Add(go_call);
            funcdec.comps.Add(new Store(new Vari("ptr", "@client"), go_v));
            var go_c2 = new Vari(this.model + "*", "%v" + LLVM.n++);
            var go_cast = new Bitcast(go_c2, go_v);
            funcdec.comps.Add(go_cast);

            var vc = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete(this.model, vc, go_c2, new Vari("i32", "0"), new Vari("i32", "0"));
            funcdec.comps.Add(gete);
            store = new Store(vc, block);
            funcdec.comps.Add(store);

            var objptr3 = new Vari(this.model + "**", "%v" + LLVM.n++);
            var alloca3 = new Alloca(objptr3);
            funcdec.comps.Add(alloca3);
            var objstore3 = new Store(objptr3, go_c2);
            funcdec.comps.Add(objstore3);
            var count = 8;
            int order = 1;

            var decs = new List<Obj>();
            foreach (var kv in blk.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                        var vdraw = new Vari("i32", "%v" + LLVM.n++);
                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                        count += 8;
                    }
                    else if (variable.cls == local.Bool)
                    {
                        var vdraw = new Vari("i1", "%v" + LLVM.n++);
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        var vdraw = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);
                }
            }
            local.blok = block;
            local.typedec = typedec;
            local.funccheck = funccheck;
            local.funcdec = funcdec;
            local.i8p = i8p;
            local.objptr3 = objptr3;
            local.co_val = co_val;
            local.countv = countv;
            local.odr = order;
            local.count = count;
            for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
            funcdec.comps.Add(new Ret(go_c2));
            funccheck.comps.Add(new Ret(new Vari("void", null)));
            var vv = new Vari(this.model + "*", "%vv" + LLVM.n++);
            func.comps.Add(new Call(vv, funcdec.y, rn, obj));
            var objptr = new Vari(model + "**", "%objptr" + LLVM.n++);
            func.comps.Add(new Alloca(objptr));
            func.comps.Add(new Store(objptr, vv));
            blk.obj.obj.bas = objptr;
            blk.exeC(local);
            local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
            local.llvm.tagparents.RemoveAt(local.llvm.tagparents.Count - 1);
            local.llvm.tagparenttes.RemoveAt(local.llvm.tagparenttes.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);
            local.comments.RemoveAt(local.comments.Count - 1);
            this.vari = func.y;
            return this;
        }
    }
    partial class LinearFunction
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                n++;
                local.elem = true;
                var val = val2.exeC(local).GetterC(local);
                local.elem = false;
                local.calls.RemoveAt(local.calls.Count - 1);
                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                var blk = val as Block;
                var anif = new AnimationFunction();
                var bools = new Boolean[blk.rets.Count];
                for (var i = 0; i < bools.Length; i++) bools[i] = false;
                foreach (var v in blk.labelmap)
                {
                    if (int.TryParse(v.Key, out int n2))
                    {
                        bools[v.Value.n] = true;
                        if (blk.rets[v.Value.n].type == ObjType.Block || blk.rets[v.Value.n].type == ObjType.Array)
                        {
                            var blk1 = blk.rets[v.Value.n] as Block;
                            TagBlock tag = null;
                            for (var i = local.blocks.Count - 1; i >= 0; i--)
                            {
                                var blk2 = local.blocks[i] as Block;
                                if (blk2.obj.obj.type == ObjType.TagBlock)
                                {
                                    tag = blk2.obj.obj as TagBlock;
                                    break;
                                }
                            }
                            anif.SetB(tag.divobj.elem, blk1, local);
                        }
                        else return Obj.Error(ObjType.Error, blk.letters[v.Value.n], "アニメーションするStyleを指定してください");
                    }
                }
                for(var i = 0; i < bools.Length; i++)
                {
                    if (bools[i]) continue;
                    var val3 = blk.rets[i];
                    if (val3.type == ObjType.FloatVal)
                    {
                        anif.interval = (val3 as FloatVal).value;
                    }
                    else if (val3.type == ObjType.Number)
                    {
                        anif.loop = (val3 as Number).value;
                    }
                    else return Obj.Error(ObjType.Error, blk.letters[0], "インターバルは小数、繰り返しの回数は整数で引数を与えてください");
                }
                return anif;
            }
            return Obj.Error(ObjType.NG, val2.letter, "linearの()が書かれていません");
        }
    }
    partial class AnimationFunction
    {
        public void SetC(Element div, Block block, Local local)
        {
            foreach (var labelmap in block.labelmap)
            {
                if (div is Div)
                {
                    (div as Div).SetParamB(labelmap.Key, block.rets[labelmap.Value.n], local);
                }
                else if (div is Span)
                {
                    (div as Span).SetParamB(labelmap.Key, block.rets[labelmap.Value.n], local);
                }
            }
        }
    }
    partial class Comment2
    {
        public override Obj exeC(Local local)
        {
            return this;
        }
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return this;
        }
    }
    partial class Clones
    {
        public Obj RightRightC(String op, Local local, Obj val2)
        {
            return this;
        }
    }
    partial class Dolor
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            n++;
            var val2 = primary.children[n];
            if (val2.type == ObjType.Word)
            {
                var word = val2 as Word;
                n++;
                switch (word.name)
                {
                    case "type":
                        var clones = new Clones();
                        clones.letter = letter;
                        return clones;
                    case "func":
                        break;
                }
            }
            else if (val2.type == ObjType.Bracket)
            {
                var val = val2.exeC(local).GetterC(local);
                local.calls.RemoveAt(local.calls.Count - 1);
                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                var blk = val as Block;
                if (blk.rets.Count != 1) return Obj.Error(ObjType.Error, val2.letter, "$()は引数を1つしかとれません");
                val2 = blk.rets[0];
                if (blk.rets[0].type == ObjType.Number || blk.rets[0].type == ObjType.StrObj)
                {
                    if (local.comments.Count > 0)
                    {
                        var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                        if (local.llvm.tagvars.Last().Length > 0)
                        {
                            var str = new Vari("%StringType*", "%str" + LLVM.n++);
                            var value = local.llvm.tagvars.Last();
                            int n2 = 1;
                            var v2 = new Vari("i8*", "%v" + LLVM.n++);
                            var sv = new StrV("@s" + LLVM.n++, value, value.Length * n2);
                            local.llvm.strs.Add(sv);
                            local.llvm.func.comps.Add(new Gete("[" + (value.Length * n2 + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                            var fv = new Vari("%CreateStringType*", "%fv" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(fv, new Vari("%CreateStringType*", "@CreateString")));
                            local.llvm.func.comps.Add(new Call(str, fv, thgc4, v2, new Vari("i32", value.Length.ToString()), new Vari("i32", n2.ToString())));
                            if (local.llvm.tagvarvars.Last() == null)
                            {
                                local.llvm.tagvarvars[local.llvm.tagvarvars.Count - 1] = str;
                                return null;
                            }
                            else
                            {
                                var fv2 = new Vari("%AddStringType2", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv2, new Vari("%AddStringType2*", "@AddString2")));
                                var str2 = new Vari("%StringType*", "%str" + LLVM.n++);
                                local.llvm.func.comps.Add(new Call(str2, fv2, thgc4, local.llvm.tagvarvars.Last(), str));
                                local.llvm.tagvarvars[local.llvm.tagvarvars.Count - 1] = str2;
                            }
                            local.llvm.tagvars[local.llvm.tagvars.Count - 1] = "";
                        }
                        {
                            var fv = new Vari("%CreateLetterType", "%fv" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(fv, new Vari("%CreateLetterType*", "@CreateLetterType")));
                            var str = new Vari("%StringType*", "%str" + LLVM.n++);
                            if (blk.rets[0].type == ObjType.Number)
                            {
                                var fv2 = new Vari("%NumberStringType", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv2, new Vari("%NumberStringType*", "@NumberString")));
                                local.llvm.func.comps.Add(new Call(str, fv2, thgc4, blk.rets[0].vari));
                            }
                            else
                            {
                                str = blk.rets[0].vari;
                            }
                            if (local.llvm.tagvarvars.Last() == null)
                            {
                                local.llvm.tagvarvars[local.llvm.tagvarvars.Count - 1] = str;
                                return null;
                            }
                            else
                            {
                                var fv2 = new Vari("%AddStringType2", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv2, new Vari("%AddStringType2*", "@AddString2")));
                                var str2 = new Vari("%StringType*", "%str" + LLVM.n++);
                                local.llvm.func.comps.Add(new Call(str2, fv2, thgc4, local.llvm.tagvarvars.Last(), str));
                                local.llvm.tagvarvars[local.llvm.tagvarvars.Count - 1] = str2;
                            }
                        }
                    }
                    else return Obj.Error(ObjType.Error, blk.letters[0], "$()は構造コメントブロックの中でしか使えません");
                }
                else return Obj.Error(ObjType.Error, blk.letters[0], "$()は整数、文字列を出力します");
            }
            return Obj.Error(ObjType.Error, val2.letter, "$の後ろには()かtype,funcが来ます");
        }
    }
    partial class HtmObj
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (local.comments.Count > 0)
            {
                local.llvm.tagvars[local.llvm.tagvars.Count - 1] = letter.name;
                return null;
            }
            return Obj.Error(ObjType.Error, letter, "コメント内で宣言してください");
        }
    }
    partial class TagBlock
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            if (local.comments.Count > 0)
            {
                var blk1 = children[0] as Block;
                int order = local.odr;
                local.blocks.Add(blk1);
                var blk2 = children[1] as Block;
                var decs = new List<Obj>();
                var thgc2 = new Vari("%ThreadGCType*", "%thgc");
                var block = local.blok;
                var typedec = local.typedec;
                var count = local.count;
                var funccheck = local.funccheck;
                var funcdec = local.funcdec;
                var i8p = local.i8p;
                var objptr3 = local.objptr3;
                var co_val = local.co_val;
                foreach (var kv in blk1.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            var vdraw = new Vari("i32", "%v" + LLVM.n++);
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            var vdraw = new Vari("i1", "%v" + LLVM.n++);
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;

                            var varii = new Vari("i1*", "%v" + LLVM.n++);
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            var vdraw = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                            typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                            count += 8;

                            var vari = new Vari("i8*", "%v" + LLVM.n++);
                            var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            funccheck.comps.Add(gete);
                            var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            var co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            var co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            var store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                        var vari = new Vari("i8*", "%v" + LLVM.n++);
                        var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        var co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        var co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        var store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;

                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                        funcdec.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        funcdec.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funcdec.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        funcdec.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                        var vari = new Vari("i8*", "%v" + LLVM.n++);
                        var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        var co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        var co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        var store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;

                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                        funcdec.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        funcdec.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funcdec.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        funcdec.comps.Add(fu_store);
                    }
                }
                foreach (var kv in blk2.vmapA)
                {
                    var v = kv.Value;
                    if (v.type == ObjType.Variable)
                    {
                        var variable = v as Variable;
                        variable.order = order++;
                        if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                        if (variable.cls == local.Int)
                        {
                            typedec.comps.Add(new TypeVal("i32", kv.Key));
                            count += 8;
                        }
                        else if (variable.cls == local.Bool)
                        {
                            typedec.comps.Add(new TypeVal("i1", kv.Key));
                            count += 8;
                        }
                        else
                        {
                            if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                            typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                            count += 8;

                            var vari = new Vari("i8*", "%v" + LLVM.n++);
                            var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                            funccheck.comps.Add(gete);
                            var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                            var co_lod = new Load(vari2, vari);
                            funccheck.comps.Add(co_lod);
                            var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                            var co_call = new Call(co_cval, co_val, thgc2, vari);
                            funccheck.comps.Add(co_call);
                            var store = new Store(vari, co_cval);
                            funccheck.comps.Add(store);
                        }
                    }
                    else if (v.type == ObjType.Function)
                    {
                        var f = v as Function;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        f.order = order++;
                        decs.Add(f);
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                        var vari = new Vari("i8*", "%v" + LLVM.n++);
                        var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        var co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        var co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        var store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;

                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        funcdec.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        funcdec.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funcdec.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        funcdec.comps.Add(fu_store);

                    }
                    else if (v.type == ObjType.ClassObj)
                    {
                        var f = v as ClassObj;
                        if (f.identity == 0) f.identity = LLVM.n++;
                        decs.Add(f);
                        f.order = order++;
                        typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                        var vari = new Vari("i8*", "%v" + LLVM.n++);
                        var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funccheck.comps.Add(gete);
                        var vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        var co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        var co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        var co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        var store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                        count += 8;

                        var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                        var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                        funcdec.comps.Add(fu_call);
                        var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                        funcdec.comps.Add(new Load(va2, objptr3));
                        var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                        funcdec.comps.Add(gete);
                        var fu_store = new Store(va, va0);
                        funcdec.comps.Add(fu_store);
                    }
                }
                local.odr = order;
                local.count = count;
                local.countv.name = count.ToString();
                for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
                var obj = blk1.exeC(local).GetterC(local) as Block ;
                var thgc = new Vari("%ThreadGCType*", "%thgc");
                if (local.llvm.tagvars.Count > 0)
                {
                    var fv = new Vari("%CreateLetterTpe*", "%fv" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(fv, new Vari("%CreateLetterTpe*", "@CreateLetter")));
                    local.llvm.func.comps.Add(new Call(null, fv, local.llvm.tagparents.Last(), local.llvm.tagvarvars.Last()));
                }
                bool id = false;
                Vari ev = null;
                if (obj.rets[0] is ElemType)
                {
                    switch (obj.rets[0].type)
                    {
                        case ObjType.Div:
                            {
                                var fv = new Vari("%CreateDivType*", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv, new Vari("%CreateDivType*", "@CreateDiv")));
                                ev = new Vari("%ElementType*", "%ev" + LLVM.n++);
                                local.llvm.func.comps.Add(new Call(ev, fv, thgc));
                                var fv4 = new Vari("%ElementAddType*", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv4, new Vari("%ElementAddType*", "@ElementAdd")));
                                local.llvm.func.comps.Add(new Call(null, fv4, thgc, local.llvm.tagparents.Count == 0 ? null: local.llvm.tagparents.Last(), ev));
                                local.llvm.tagparents.Add(ev);
                            }
                            break;
                        case ObjType.Sheet:
                            {
                                var fv = new Vari("%CreateDivType*", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv, new Vari("%CreateDivType*", "@CreateSheet")));
                                ev = new Vari("%ElementType*", "%ev" + LLVM.n++);
                                local.llvm.func.comps.Add(new Call(ev, fv, thgc));
                                var fv4 = new Vari("%ElementAddType*", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv4, new Vari("%ElementAddType*", "@ElementAdd")));
                                local.llvm.func.comps.Add(new Call(null, fv4, thgc, local.llvm.tagparents.Count == 0 ? null : local.llvm.tagparents.Last(), ev));
                                local.llvm.tagparents.Add(ev);
                            }
                            break;
                        case ObjType.Br:
                            var fv2 = new Vari("%CreateKaigyouType*", "%fv" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(fv2, new Vari("%CreateKaigyouType*", "@CreateKaigyou")));
                            var ev2 = new Vari("%ElementType*", "%ev" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(ev2, fv2, local.llvm.tagparents.Count == 0 ? new Vari("%ElementType*", "null"): local.llvm.tagparents.Last()));
                            return this;
                        case ObjType.Span:
                            //create span element
                            break;
                    }
                }
                else
                {
                    id = true;
                    var elemobj = obj.rets[0] as ElemObj;
                    if (elemobj.id == "main")
                    {

                        var fv = new Vari("%CreateDivType*", "%fv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(fv, new Vari("%CreateDivType*", "@CreateDiv")));
                        var fv4 = new Vari("%CheckTreeElementType*", "%fv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(fv4, new Vari("%CheckTreeElementType*", "@CheckTreeElement")));
                        var tev = new Vari("%TreeElementType*", "%tev" + LLVM.n++);
                        var deletes = new Vari("%ListType*", "%deletes");
                        var crs = new Vari("%CreateStringType", "%crs" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(crs, new Vari("%CreateStringType*", "@CreateString")));
                        var strv = new StrV("@s" + LLVM.n++, elemobj.id, elemobj.id.Length);
                        local.llvm.strs.Add(strv);
                        var sv = new Vari("%StringType*", "%sv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Call(sv, crs, thgc, strv, new Vari("i32", elemobj.id.Length.ToString()), new Vari("i32", "1")));
                        local.llvm.func.comps.Add(new Call(tev, fv4, thgc, local.llvm.tagparenttes.Count == 0 ? new Vari("%TreeElementType", "null") : local.llvm.tagparenttes.Last(), local.llvm.tagparents.Count == 0 ? new Vari("%ElementType*", "null") : local.llvm.tagparents.Last(), deletes, sv, new Vari("i32", "2"), new Vari("i32", elemobj.op.ToString()), fv));
                        var evp = new Vari("%ElementType**", "%evp" + LLVM.n++);
                        local.llvm.func.comps.Add(new Gete("%TreeElementType", evp, tev, new Vari("i32", "0"), new Vari("i32", "1")));
                        ev = new Vari("%ElementType*", "%ev" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(ev, evp));
                        local.llvm.tagparents.Add(ev);
                        local.llvm.tagparenttes.Add(tev);
                    }
                    else switch (elemobj.type.type)
                    {
                        case ObjType.Div:
                            {
                                var fv = new Vari("%CreateDivType*", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv, new Vari("%CreateDivType*", "@CreateDiv")));
                                var fv4 = new Vari("%CheckTreeElementType*", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv4, new Vari("%CheckTreeElementType*", "@CheckTreeElement")));
                                var tev = new Vari("%TreeElementType*", "%tev" + LLVM.n++);
                                var deletes = new Vari("%ListType*", "%deletes");
                                var crs = new Vari("%CreateStringType", "%crs" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(crs, new Vari("%CreateStringType*", "@CreateString")));
                                var strv = new StrV("@s" + LLVM.n++, elemobj.id, elemobj.id.Length);
                                local.llvm.strs.Add(strv);
                                var sv = new Vari("%StringType*", "%sv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Call(sv, crs, thgc, strv, new Vari("i32", elemobj.id.Length.ToString()), new Vari("i32", "1")));
                                local.llvm.func.comps.Add(new Call(tev, fv4, thgc, local.llvm.tagparenttes.Count == 0 ? new Vari("%TreeElementType", "null") : local.llvm.tagparenttes.Last(), local.llvm.tagparents.Count == 0 ? new Vari("%ElementType*", "null") : local.llvm.tagparents.Last(), deletes, sv, new Vari("i32", "21"), new Vari("i32", elemobj.op.ToString()), fv));
                                var evp = new Vari("%ElementType**", "%evp" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%TreeElementType", evp, tev, new Vari("i32", "0"), new Vari("i32", "1")));
                                ev = new Vari("%ElementType*", "%ev" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(ev, evp));
                                local.llvm.tagparents.Add(ev);
                                local.llvm.tagparenttes.Add(tev);
                            }
                            break;
                        case ObjType.Sheet:
                            {
                                var fv = new Vari("%CreateDivType*", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv, new Vari("%CreateDivType*", "@CreateSheet")));
                                var fv4 = new Vari("%CheckTreeElementType*", "%fv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(fv4, new Vari("%CheckTreeElementType*", "@CheckTreeElement")));
                                var tev = new Vari("%TreeElementType*", "%tev" + LLVM.n++);
                                var deletes = new Vari("%ListType*", "%deletes");
                                var crs = new Vari("%CreateStringType", "%crs" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(crs, new Vari("%CreateStringType*", "@CreateString")));
                                var strv = new StrV("@s" + LLVM.n++, elemobj.id, elemobj.id.Length);
                                local.llvm.strs.Add(strv);
                                var sv = new Vari("%StringType*", "%sv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Call(sv, crs, thgc, strv, new Vari("i32", elemobj.id.Length.ToString()), new Vari("i32", "1")));
                                local.llvm.func.comps.Add(new Call(tev, fv4, thgc, local.llvm.tagparenttes.Count == 0 ? new Vari("%TreeElementType", "null") : local.llvm.tagparenttes.Last(), local.llvm.tagparents.Count == 0 ? new Vari("%ElementType*", "null") : local.llvm.tagparents.Last(), deletes, sv, new Vari("i32", "21"), new Vari("i32", elemobj.op.ToString()), fv));
                                var evp = new Vari("%ElementType**", "%evp" + LLVM.n++);
                                local.llvm.func.comps.Add(new Gete("%TreeElementType", evp, tev, new Vari("i32", "0"), new Vari("i32", "1")));
                                ev = new Vari("%ElementType*", "%ev" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(ev, evp));
                                local.llvm.tagparents.Add(ev);
                                local.llvm.tagparenttes.Add(tev);
                            }
                            break;
                        case ObjType.Br:
                            var fv2 = new Vari("%CreateKaigyouType*", "%fv" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(fv2, new Vari("%CreateKaigyouType*", "@CreateKaigyou")));
                            var ev2 = new Vari("%ElementType*", "%ev" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(ev2, fv2, local.llvm.tagparents.Last()));
                            return this;
                        case ObjType.Span:
                            //create span element
                            break;
                    }
                    //create treeelement
                }
                for (int i = 1; i < blk1.rets.Count; i++)
                {
                    var val = blk1.rets[i];
                    if (blk1.labelmapn.ContainsKey(i))
                    {
                        var label = blk1.labelmapn[i];
                        switch (label.name)
                        {
                            case "left":
                                {
                                    var ip = new Vari("i32*", "%ip" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ElementType", ip, ev, new Vari("i32", "0"), new Vari("i32", "6"), new Vari("i32", "0")));
                                    local.llvm.func.comps.Add(new Store(ip, blk1.rets[i].vari));
                                }
                                //element.x = blk1.rets[i].vari
                                break;
                            case "top":
                                {
                                    var ip = new Vari("i32*", "%ip" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ElementType", ip,ev, new Vari("i32", "0"), new Vari("i32", "6"), new Vari("i32", "1")));
                                    local.llvm.func.comps.Add(new Store(ip, blk1.rets[i].vari));
                                }
                                //element.y = blk1.rets[i].vari
                                break;
                            case "p":
                                //element.x = blk1.rets[i].vari[0]
                                //element.y = blk1.rets[i].vari[1]
                                break;
                            case "w":
                            case "width":
                                //element.width = blk1.rets[i].vari
                                break;
                            case "h":
                            case "height":
                                //element.height = blk1.rets[i].vari
                                break;
                            case "size":
                                //element.width = blk1.rets[i].vari[0]
                                //element.height = blk1.rets[i].vari[1]
                                break;
                            case "xtype":
                                break;
                            case "ytype":
                                break;
                            case "ptype":
                                break;
                            case "pos":
                            case "position":
                                break;
                            case "c":
                            case "color":
                                break;
                            case "b":
                            case "background":
                                break;
                            case "gomouseup":
                                break;
                            case "gomousedown":
                                if (val.type == ObjType.SignalFunction)
                                {
                                    var mvp = new Vari("%MemFuncType**", "%mvp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ElementType", mvp, ev, new Vari("i32", "0"), new Vari("i32", "26")));
                                    local.llvm.func.comps.Add(new Store(mvp, blk1.rets[i].vari));
                                }
                                break;
                            case "backmousedown":
                                if (val.type == ObjType.SignalFunction)
                                {
                                    var mvp = new Vari("%MemFuncType**", "%mvp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ElementType", mvp, ev, new Vari("i32", "0"), new Vari("i32", "27")));
                                    local.llvm.func.comps.Add(new Store(mvp, blk1.rets[i].vari));
                                }
                                break;
                            case "gokeydown":
                                if (val.type == ObjType.SignalFunction)
                                {
                                    var mvp = new Vari("%MemFuncType**", "%mvp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ElementType", mvp, ev, new Vari("i32", "0"), new Vari("i32", "28")));
                                    local.llvm.func.comps.Add(new Store(mvp, blk1.rets[i].vari));
                                }
                                break;
                            case "backkeydown":
                                if (val.type == ObjType.SignalFunction)
                                {
                                    var mvp = new Vari("%MemFuncType**", "%mvp" + LLVM.n++);
                                    local.llvm.func.comps.Add(new Gete("%ElementType", mvp, ev, new Vari("i32", "0"), new Vari("i32", "29")));
                                    local.llvm.func.comps.Add(new Store(mvp, blk1.rets[i].vari));
                                }
                                break;
                            case "keyup":
                            case "keyinput":
                                break;
                            default:
                                //element.SetParam(label.name, blk1.rets[i].vari)
                                break;
                        }
                    }

                }
                local.llvm.tagvars.Add("");
                local.llvm.tagvarvars.Add(null);
                local.blocks.Add(blk2);
                children[1].exeC(local);
                var thgc4 = new Vari("%ThreadGCType*", "%thgc");
                if (ev != null)
                {
                    if (local.llvm.tagvars.Last().Length > 0)
                    {
                        var str = new Vari("%StringType*", "%str" + LLVM.n++);
                        var value = local.llvm.tagvars.Last();
                        int n2 = 1;
                        var v2 = new Vari("i8*", "%v" + LLVM.n++);
                        var sv = new StrV("@s" + LLVM.n++, value, value.Length * n2);
                        local.llvm.strs.Add(sv);
                        local.llvm.func.comps.Add(new Gete("[" + (value.Length * n2 + 1) + " x i8]", v2, sv, new Vari("i32", "0"), new Vari("i32", "0")));
                        var fv = new Vari("%CreateStringType*", "%fv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(fv, new Vari("%CreateStringType*", "@CreateString")));
                        local.llvm.func.comps.Add(new Call(str, fv, thgc4, v2, new Vari("i32", value.Length.ToString()), new Vari("i32", n2.ToString())));
                        if (local.llvm.tagvarvars.Last() == null)
                        {
                            local.llvm.tagvarvars[local.llvm.tagvarvars.Count - 1] = str;
                        }
                        else
                        {
                            var fv2 = new Vari("%AddStringType2", "%fv" + LLVM.n++);
                            local.llvm.func.comps.Add(new Load(fv2, new Vari("%AddStringType2*", "@AddString2")));
                            var str2 = new Vari("%StringType*", "%str" + LLVM.n++);
                            local.llvm.func.comps.Add(new Call(str2, fv2, thgc4, local.llvm.tagvarvars.Last(), str));
                            local.llvm.tagvarvars[local.llvm.tagvarvars.Count - 1] = str2;
                        }
                        local.llvm.tagvars[local.llvm.tagvars.Count - 1] = "";
                    }
                    if (local.llvm.tagvarvars.Last() != null)
                    {
                        var fv4 = new Vari("%CreateLetterType", "%fv" + LLVM.n++);
                        local.llvm.func.comps.Add(new Load(fv4, new Vari("%CreateLetterType*", "@CreateLetter")));
                        local.llvm.func.comps.Add(new Call(null, fv4, thgc4, local.llvm.tagparents.Last(), local.llvm.tagvarvars.Last()));

                    }
                    var fv3 = new Vari("%CreateEndType", "%fv" + LLVM.n++);
                    local.llvm.func.comps.Add(new Load(fv3, new Vari("%CreateEndType*", "@CreateEnd")));
                    var ev3 = new Vari("%ElementType*", "%ev" + LLVM.n++);
                    local.llvm.func.comps.Add(new Call(ev3, fv3, thgc4, local.llvm.tagparents.Last()));
                }
                local.blocks.RemoveAt(local.blocks.Count - 1);
                local.blocks.RemoveAt(local.blocks.Count - 1);
                local.llvm.tagvars.RemoveAt(local.llvm.tagvars.Count - 1);
                local.llvm.tagvarvars.RemoveAt(local.llvm.tagvarvars.Count - 1);
                local.llvm.tagparents.RemoveAt(local.llvm.tagparents.Count - 1);
                if (id) local.llvm.tagparenttes.RemoveAt(local.llvm.tagparenttes.Count - 1);
            }
            return this;
        }
        public override Obj exeC(Local local)
        {
            return this;
        }
    }
    partial class ElemType
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            var val2 = primary.children[n + 1];
            if (val2.type == ObjType.Word)
            {
                n++;
                var word = val2 as Word;
                var elemobj = new ElemObj(local, this, word.name);
                //local.declareC(word.name, elemobj);
                return elemobj;
            }
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Word)
            {
                var word = val2 as Word;
                n++;
                var elemobj = new ElemObj(local, this, word.name);
                local.declareB(word.name, elemobj);
                return elemobj;
            }
            return Obj.Error(ObjType.NG, val2.letter, "elem変数の名前が宣言されていません");
        }
    }
    partial class ElemObj
    {
        public int op = 0;
        public void paramC(String name, Obj obj, Local local)
        {
            if (elem is Div)
            {
                (elem as Div).SetParamB(name, obj, local);
            }
            else if (elem is Span)
            {
                (elem as Span).SetParamB(name, obj, local);
            }
        }
        public override Obj opeC(string key, Local local, Obj val2)
        {
            if ((key == "+" || key == "-" || key == "*" || key == "/") && val2 == null)
            {
                if (key == "+") op |= 1;
                else if (key == "-") op |= 2;
                else if (key == "*") op |= 4;
                else if (key == "/") op |= 8;
                if (type.type == ObjType.Div || type.type == ObjType.Sheet || type.type == ObjType.Cell)
                {
                    (elem as Div).sop = key;
                }
                this.key = key;
                return this;
            }
            return Obj.Error(ObjType.Error, letter, "elem値は+,!,*の単項演算子しか作用しません");
        }
    }
    partial class CDec
    {
    }
    partial class CFunc
    {
    }
    partial class CType
    {
    }
    partial class Signal
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Word)
            {
                var word = val2 as Word;
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.CallBlock)
                {
                    n++;
                    SignalFunction func = null;
                    for (var i = local.blocks.Count - 1; i > 0; i--)
                    {
                        if (local.blocks[i].obj.obj.type == ObjType.ServerFunction)
                        {
                            return func;
                        }
                    }
                    return Obj.Error(ObjType.Error, letter, "signal関数はserver{|},client{|}の中でしか宣言出来ません");
                }
                return Obj.Error(ObjType.Error, val2.letter, "signal関数に{|}が宣言されていない");
            }
            return Obj.Error(ObjType.Error, val2.letter, "signal関数を宣言してください");
        }
    }
    partial class SignalFunction
    {
        public int identity = 0;
        public override Obj exeC(Local local)
        {
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            return Obj.Error(ObjType.NG, val2.letter, "signal関数はユーザーから呼び出せません");
        }
        public override Obj GetterC(Local local)
        {
            for (var i = 0; i < local.comps.Count; i++) local.llvm.func.comps.Add(local.comps[i]);
            local.comps = new List<Component>();
            var fv = new Vari("%MemFuncType*", "%v" + LLVM.n++);
            local.llvm.func.vari.type = "%MemFuncType**";
            var load = new Load(fv, local.llvm.func.vari);
            local.llvm.func.comps.Add(load);
            var sf = Clone();
            sf.vari = fv;
            return sf;
        }
        public override void Dec(Local local)
        {
            if (identity == 0) identity = LLVM.n++;
            ifv = new Dictionary<string, IfValue>();
            String type;
            this.n = Obj.cn++;
            var callname = "@" + this.letter.name + "SigFunc" + identity;
            this.vari = new Vari("void", callname);
            var obj = new Vari("%CoroFrameType*", "%obj");
            var func = new Func(local.llvm, this.vari, obj);
            func.async = true;
            this.model = "%mmm" + identity;

            var rn6 = new Vari("%RootNodeType*", "%rn");
            var block = new Vari("i8*", "%block");
            var decname = "@" + this.letter.name + "dec" + identity;
            var funcdec = new Func(local.llvm, new Vari("%MemFuncType*", decname), rn6, block);
            local.llvm.comps.Add(funcdec);
            var thgcptr6 = new Vari("%ThreadGCType**", "%thgcptr");
            funcdec.comps.Add(new Gete("%RootNodeType", thgcptr6, rn6, new Vari("i32", "0"), new Vari("i32", "0")));
            var thgc6 = new Vari("%ThreadGCType*", "%thgc");
            funcdec.comps.Add(new Load(thgc6, thgcptr6));

            var root = new Vari("%RootNodeType*", "%rn");
            var oj = new Vari("%GCObjectPtr", "%oj" + LLVM.n++);
            var ov = new Vari("%GCObjectPtr", "%ov" + LLVM.n++);
            /*if (local.llvm.func.async)
            {
                var objf = new Vari("%CoroFrameType*", "%frame");
                var ogv = new Vari("%GCObjectPtr*", "%obj" + LLVM.n++);
                local.llvm.func.comps.Add(new Gete("%CoroFrameType", ogv, objf, new Vari("i32", "0"), new Vari("i32", "5")));
                local.llvm.func.comps.Add(new Load(oj, ogv));
            }
            else if (local.blocks.Last().obj.obj.type == ObjType.IfBlock)
            {
                local.llvm.func.comps.Add(new Load(oj, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
            }
            else
            {
                local.llvm.func.comps.Add(new Load(oj, local.blocks.Last().obj.obj.bas));
            }
            local.llvm.func.comps.Add(new Call(ov, funcdec.y, root, oj));
            //Make root node
            var hv = new Vari("%CoroFrameType*", "%hv");
            local.llvm.func.comps.Add(new Call(new Vari("ptr", hv.name), new Vari("ptr", "@MakeFrame"), root, new Vari("ptr", func.y.name), ov));
            var statevalptr22 = new Vari("i32*", "%stateptr" + LLVM.n++);
            local.llvm.func.comps.Add(new Gete("%CoroFrameType", statevalptr22, hv, new Vari("i32", "0"), new Vari("i32", "7")));
            var nvari = new Vari("i32", (LLVM.n++).ToString());
            local.llvm.func.comps.Add(new Store(statevalptr22, new Vari("i32", "-2")));
            var gcr = new Vari("%GC_AddRootType", "%gcr" + LLVM.n++);
            local.llvm.func.comps.Add(new Load(gcr, new Vari("%GC_AddRootType*", "@GC_AddRoot")));
            var rv = new Vari("%RootNodeType*", "%rv" + LLVM.n++);
            var thgc = new Vari("%ThreadGCType*", "%thgc");
            local.llvm.func.comps.Add(new Call(rv, gcr, thgc));
            var rvp = new Vari("%RootNodeType**", "%rvp" + LLVM.n++);
            local.llvm.func.comps.Add(new Gete("%CoroFrameType", rvp, hv, new Vari("i32", "0"), new Vari("i32", "3")));
            local.llvm.func.comps.Add(new Store(rvp, rv));
            //make roots
            //push handle
            var phv = new Vari("%PushQueueType", "%phv" + LLVM.n++);
            local.llvm.func.comps.Add(new Load(phv, new Vari("%PushQueueType*", "@PushQueue")));
            var qv = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
            local.llvm.func.comps.Add(new Gete("%ThreadGCType", qv, thgc, new Vari("i32", "0"), new Vari("i32", "0")));
            var qv2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
            local.llvm.func.comps.Add(new Load(qv2, qv));
            local.llvm.func.comps.Add(new Call(null, phv, qv2, hv));*/

            local.llvm.comps.Add(func);
            local.llvm.funcs.Add(func);
            //normal; make roots;make handle; push handle;| await; make Coroutine Queue;make handle;push handle;| In Cotoutine[nomarl; make roots;make handle; push handle;| await; make handle; give old-handle; push handle;on-final; exe old-handle;]
            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
            var rngv = new Vari("%RootNodeType**", "%rngv");
            func.comps.Add(new Gete("%CoroFrameType", rngv, obj, new Vari("i32", "0"), new Vari("i32", "3")));
            var rn4 = new Vari("%RootNodeType*", "%rn");
            func.comps.Add(new Load(rn4, rngv));
            func.comps.Add(new Load(thgc4, new Vari("%ThreadGCType**", "@thgcp")));

            var geterv = new Vari("i32*", "%state");
            func.comps.Add(new Gete("%CoroFrameType", geterv, obj, new Vari("i32", "0"), new Vari("i32", "7")));
            var statev = new Vari("i32", "%statev" + LLVM.n++);
            func.comps.Add(new Load(statev, geterv));
            var list = new List<IfValue>();
            var lab10 = new Lab("coro_tx");
            list.Add(new IfValue(lab10, new Vari("i32", "-2")));
            var lab11 = new Lab("coro_start");
            list.Add(new IfValue(lab11, new Vari("i32", "0")));
            func.sc = new SwitchComp(statev, lab11, list);
            func.comps.Add(func.sc);

            var checkname = "@" + this.letter.name + "Check" + identity;
            var thgc3 = new Vari("%ThreadGCType*", "%thgc");
            var ac_val = new Vari("%GC_AddClassType", "%addclass");
            var countv = new Vari("i32", "0");
            var name = "async" + LLVM.n++;
            var strv = new StrV("@" + name, name, name.Length * 1);
            local.llvm.strs.Add(strv);
            var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
            var tnp = new Vari("i32", "%tnp" + LLVM.n++);
            local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
            var tv = new Vari("i32", "%cv" + LLVM.n++);
            local.llvm.main.comps.Add(new Add(tv, tnp, typ));
            var ac_call = new Call(null, ac_val, thgc3, tv, countv, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
            local.llvm.main.comps.Add(ac_call);

            var gmvari = new Vari("%GC_mallocType", "%gm");
            funcdec.comps.Add(new Load(gmvari, new Vari("%GC_mallocType*", "@GC_malloc")));
            var go_m = new Vari("%MemFuncType*", "%m" + LLVM.n++);
            funcdec.comps.Add(new Call(go_m, gmvari, thgc4, new Vari("i32", 27.ToString())));
            var vp1 = new Vari("ptr", "%vp" + LLVM.n++);
            funcdec.comps.Add(new Gete("%MemFuncType", vp1, go_m, new Vari("i32", "0"), new Vari("i32", "0")));
            funcdec.comps.Add(new Store(vp1, block));
            var vp2 = new Vari("ptr", "%vp" + LLVM.n++);
            funcdec.comps.Add(new Gete("%MemFuncType", vp2, go_m, new Vari("i32", "0"), new Vari("i32", "2")));
            funcdec.comps.Add(new Store(vp2, new Vari("ptr", func.y.name)));

            var go_v = new Vari("%GCObjectPtr", "%v" + LLVM.n++);
            var tmp = new Vari("i32", this.n.ToString());
            var tmi = new Vari("i32", "%tmi" + LLVM.n++);
            funcdec.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
            var tmv = new Vari("i32", "%tmv" + LLVM.n++);
            funcdec.comps.Add(new Add(tmv, tmi, tmp));
            var go_call = new Call(go_v, gmvari, thgc4, tmv);
            funcdec.comps.Add(go_call);

            var vc = new Vari("i8*", "%v" + LLVM.n++);
            var gete = new Gete(this.model, vc, go_v, new Vari("i32", "0"), new Vari("i32", "0"));
            funcdec.comps.Add(gete);
            funcdec.comps.Add(new Store(vc, block));
            var vp3 = new Vari("ptr", "%vp" + LLVM.n++);
            funcdec.comps.Add(new Gete("%MemFuncType", vp3, go_m, new Vari("i32", "0"), new Vari("i32", "1")));
            funcdec.comps.Add(new Store(vp3, vc));

            func.comps.Add(lab10);
            var pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
            func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
            var qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
            func.comps.Add(new Gete("%ThreadGCType", qvn, thgc6, new Vari("i32", "0"), new Vari("i32", "0")));
            var qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
            func.comps.Add(new Load(qvn2, qvn));
            var queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
            func.comps.Add(new Store(queueptr2, qvn2));
            var hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
            var bt = new Vari("%BeginTransactionType", "%bt");
            var alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
            func.comps.Add(new Alloca(alv));
            func.comps.Add(new Load(bt, new Vari("%BeginTransactionType*", "@BeginTransaction")));
            var sqlv = new Vari("ptr", "%sql");
            func.comps.Add(new Load(sqlv, new Vari("ptr", "@sqlp")));
            func.comps.Add(new Call(null, bt, alv, thgc6, sqlv, qvn2));
            func.comps.Add(new Load(hvn, alv));
            var parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
            func.comps.Add(new Store(parent2, func.draws[0] as Vari));
            var qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
            func.comps.Add(new Store(qp, qvn2));
            func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
            var statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
            func.comps.Add(new Store(statevalptr2, new Vari("i32", "0")));
            var sfin = new Vari("i32*", "%sfin" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", sfin, obj, new Vari("i32", "0"), new Vari("i32", "10")));
            func.comps.Add(new Store(sfin, new Vari("i32", "-3")));
            func.comps.Add(new Ret(new Vari("void", null)));

            func.comps.Add(lab11);
            var txp2 = new Vari("i8**", "%txp" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", txp2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "8")));
            var tx = new Vari("i8*", "%txv" + LLVM.n++);
            func.comps.Add(new Load(tx, txp2));
            var txptr = new Vari("i8**", "%tx" + LLVM.n++);
            func.comps.Add(new Gete("%RootNodeType", txptr, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
            func.comps.Add(new Store(txptr, tx));
            var gev = new Vari(this.model + "**", "%gv");
            func.comps.Add(new Gete("%CoroFrameType", gev, obj, new Vari("i32", "0"), new Vari("i32", "5")));
            var gev2 = new Vari(this.model + "*", "%gv2");
            func.comps.Add(new Load(gev2, gev));
            var objptr = new Vari(this.model + "**", "%objptr");
            var alloca = new Alloca(objptr);
            func.comps.Add(alloca);
            var objstore = new Store(objptr, gev2);
            func.comps.Add(objstore);
            var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
            var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
            func.comps.Add(srload);
            var srcall = new Call(null, srv, rn4, objptr);
            func.comps.Add(srcall);

            var typedec = new TypeDec(this.model);
            local.llvm.types.Add(typedec);
            typedec.comps.Add(new TypeVal("i8*", "blk"));

            var thgc2 = new Vari("%ThreadGCType*", "%thgc");
            var i8p = new Vari("i8*", "%self");
            var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
            local.llvm.comps.Add(funccheck);
            var vari = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
            funccheck.comps.Add(gete);
            var co_val = new Vari("%CopyObjectType", "%co");
            var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
            funccheck.comps.Add(co_load);
            var vari2 = new Vari("i8*", "%v" + LLVM.n++);
            var co_lod = new Load(vari2, vari);
            funccheck.comps.Add(co_lod);
            var co_cval = new Vari("i8*", "%v" + LLVM.n++);
            var co_call = new Call(co_cval, co_val, thgc2, vari);
            funccheck.comps.Add(co_call);
            var store = new Store(vari, co_cval);
            funccheck.comps.Add(store);

            int count = 8;
            var blk0 = draw.children[0] as Block;
            int order = 1;
            var blk2 = draw.children[1] as Block;
            var decs = new List<Obj>();

            foreach (var kv in blk0.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                        var vdraw = new Vari("i32", "%v" + LLVM.n++);
                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                        count += 8;
                    }
                    else if (variable.cls == local.Bool)
                    {
                        var vdraw = new Vari("i1", "%v" + LLVM.n++);
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        var vdraw = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, go_v));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, go_v));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);
                }
            }
            foreach (var kv in blk2.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                        count += 8;
                    }
                    else if (variable.cls == local.Bool)
                    {
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, go_v));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    decs.Add(f);
                    f.order = order++;
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, go_v));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);
                }
            }
            for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
            local.llvm.funcs.Add(funcdec);
            local.blocks.Add(blk0);
            //blk0.exeC(local);
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
            funcdec.comps.Add(new Ret(go_m));
            funccheck.comps.Add(new Ret(new Vari("void", null)));
            countv.name = count.ToString();
            this.bas = objptr;
            local.blocks.Add(blk2);

            local.labs.Add(new Lab("entry"));
            blk2.exeC(local);
            local.labs.RemoveAt(local.labs.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);

            statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
            var sfin2 = new Vari("i32*", "%sfin" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", sfin2, obj, new Vari("i32", "0"), new Vari("i32", "10")));
            var sfinv = new Vari("i32", "%sfinv" + LLVM.n++);
            func.comps.Add(new Load(sfinv, sfin2));
            func.comps.Add(new Store(statevalptr2, sfinv));
            func.comps.Add(new Ret(new Vari("void", null)));
            lab10 = new Lab("coro_end");
            list.Add(new IfValue(lab10, new Vari("i32", "-3")));
            func.comps.Add(lab10);
            pushqueue1 = new Vari("%PushQueueType", "%pushqueue" + LLVM.n++);
            func.comps.Add(new Load(pushqueue1, new Vari("%PushQueueType*", "@PushQueue")));
            qvn = new Vari("%CoroutineQueueType**", "%qv" + LLVM.n++);
            func.comps.Add(new Gete("%ThreadGCType", qvn, thgc4, new Vari("i32", "0"), new Vari("i32", "0")));
            qvn2 = new Vari("%CoroutineQueueType*", "%qv2" + LLVM.n++);
            func.comps.Add(new Load(qvn2, qvn));
            queueptr2 = new Vari("%CoroutineQueueType**", "%queueptr2" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", queueptr2, obj, new Vari("i32", "0"), new Vari("i32", "4")));
            func.comps.Add(new Store(queueptr2, qvn2));
            hvn = new Vari("%CoroFrameType*", "%hv" + LLVM.n++);
            bt = new Vari("%TxFinishType", "%tf");
            alv = new Vari("%CoroFrameType**", "%alv" + LLVM.n++);
            func.comps.Add(new Alloca(alv));
            func.comps.Add(new Load(bt, new Vari("%TxFinishType*", "@TxCommit")));
            var txp = new Vari("i8**", "%txp" + LLVM.n++);
            func.comps.Add(new Gete("%RootNodeType", txp, rn4, new Vari("i32", "0"), new Vari("i32", "2")));
            var txv = new Vari("i8*", "%txv" + LLVM.n++);
            func.comps.Add(new Load(txv, txp));
            func.comps.Add(new Call(null, bt, alv, txv));
            func.comps.Add(new Load(hvn, alv));
            parent2 = new Vari("%CoroFrameType**", "%parent" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", parent2, hvn, new Vari("i32", "0"), new Vari("i32", "6")));
            func.comps.Add(new Store(parent2, func.draws[0] as Vari));
            qp = new Vari("%CoroutineQueueType**", "%parent" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", qp, hvn, new Vari("i32", "0"), new Vari("i32", "4")));
            func.comps.Add(new Store(qp, qvn2));
            func.comps.Add(new Call(null, pushqueue1, qvn2, hvn));
            statevalptr2 = new Vari("i32*", "%stateptr" + LLVM.n++);
            func.comps.Add(new Gete("%CoroFrameType", statevalptr2, func.draws[0] as Vari, new Vari("i32", "0"), new Vari("i32", "7")));
            func.comps.Add(new Store(statevalptr2, new Vari("i32", "-3")));
            func.comps.Add(new Ret(new Vari("void", null)));
            lab10 = new Lab("coro_fin");
            list.Add(new IfValue(lab10, new Vari("i32", "-1")));
            func.comps.Add(lab10);

            var rnpv = new Vari("i32*", "%ptr");
            local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
            var rnpv12 = new Vari("i32", "%v");
            rnpv12 = new Vari("i32", "%v");
            var rnp12load = new Load(rnpv12, rnpv);
            local.llvm.func.comps.Add(rnp12load);
            var rnpv2 = new Vari("i32", "%dec");
            var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
            local.llvm.func.comps.Add(rnpsub);
            local.llvm.func.comps.Add(new Store(rnpv, rnpv2));
            /*var retval = new Vari("i8*", "%ret");
            var rgete = new Gete("%CoroFrameType", retval, obj, new Vari("i32", "0"), new Vari("i32", "5"));
            local.llvm.func.comps.Add(rgete);
            local.llvm.func.comps.Add(new Store(retval, blk2.vari));*/
            var stateval = new Vari("i32*", "%state" + LLVM.n++);
            local.llvm.func.comps.Add(new Gete("%CoroFrameType", stateval, obj, new Vari("i32", "0"), new Vari("i32", "7")));
            local.llvm.func.comps.Add(new Store(stateval, new Vari("i32", "-1")));
            local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
            //n++;
            //return new VoiVal();
        }
    }
    partial class ServerClient
    {
        public CallBlock draw;
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    var word = val2 as Word;
                    n++;
                    return local.sigmapA[name + ":" + word.name];
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の後には名前がきます");
            }
            else if (val2.type == ObjType.Word)
            {
                var word = val2 as Word;
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.CallBlock)
                {
                    n++;
                    var sf = local.sigmapA[name + ":" + word.name] as ServerFunction;
                    return sf;
                }
                else return Obj.Error(ObjType.Error, val2.letter, "server,clientの{|}が宣言されていません");
            }
            else if (val2.type == ObjType.CallBlock)
            {
                draw = val2 as CallBlock;
                n++;
                var sf = local.sigmapA[name] as ServerFunction;
                sf.Dec(local);
                return sf;
            }
            return Obj.Error(ObjType.Error, val2.letter, "server,clientの後ろには名前か{|}がきます");
        }
    }
    partial class ServerFunction
    {
        public override Obj exeC(Local local)
        {
            return this;
        }
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            return Obj.Error(ObjType.NG, letter, "ServerClient関数は呼び出せません");
        }
        public override string model
        {
            get { return "%" + this.letter.name + "Type"; }
        }
        public override string call
        {
            get { return "@" + this.letter.name + "Call"; }
        }
        public override string drawcall
        {
            get { return "@" + this.letter.name + "Draw"; }
        }
        public override Obj GetterC(Local local)
        {
            return this;
        }
        public override void Dec(Local local)
        {
            ifv = new Dictionary<string, IfValue>();
            this.n = Obj.cn++;
            this.vari = new Vari("void", call);
            var rn4 = new Vari("%RootNodeType*", "%rn");
            var obj = new Vari(this.model + "*", "%obj");
            var func = new Func(local.llvm, this.vari, rn4, obj);
            local.llvm.comps.Add(func);
            local.llvm.funcs.Add(func);
            var thgcptr4 = new Vari("%ThreadGCType**", "%thgcptr");
            func.comps.Add(new Gete("%RootNodeType", thgcptr4, rn4, new Vari("i32", "0"), new Vari("i32", "0")));
            var thgc4 = new Vari("%ThreadGCType*", "%thgc");
            func.comps.Add(new Load(thgc4, thgcptr4));

            var objptr = new Vari(this.model + "**", "%objptr");
            var alloca = new Alloca(objptr);
            func.comps.Add(alloca);
            var objstore = new Store(objptr, obj);
            func.comps.Add(objstore);
            var srv = new Vari("%GC_SetRootType", "%v" + LLVM.n++);
            var srload = new Load(srv, new Vari("%GC_SetRootType*", "@GC_SetRoot"));
            func.comps.Add(srload);
            var srcall = new Call(null, srv, rn4, objptr);
            func.comps.Add(srcall);

            var typedec = new TypeDec(this.model);
            local.llvm.types.Add(typedec);
            typedec.comps.Add(new TypeVal("i8*", "blk"));

            var thgc2 = new Vari("%ThreadGCType*", "%thgc");
            var i8p = new Vari("i8*", "%self");
            var checkname = "@" + this.letter.name + "Check";
            var funccheck = new Func(local.llvm, new Vari("void", checkname), thgc2, i8p);
            local.llvm.comps.Add(funccheck);
            var vari = new Vari("i8*", "%v" + LLVM.n++);
            var gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", "0"));
            funccheck.comps.Add(gete);
            var co_val = new Vari("%CopyObjectType", "%co");
            var co_load = new Load(co_val, new Vari("%CopyObjectType*", "@CopyObject"));
            funccheck.comps.Add(co_load);
            var vari2 = new Vari("i8*", "%v" + LLVM.n++);
            var co_lod = new Load(vari2, vari);
            funccheck.comps.Add(co_lod);
            var co_cval = new Vari("i8*", "%v" + LLVM.n++);
            var co_call = new Call(co_cval, co_val, thgc2, vari);
            funccheck.comps.Add(co_call);
            var store = new Store(vari, co_cval);
            funccheck.comps.Add(store);

            var thgc3 = new Vari("%ThreadGCType*", "%thgc");
            var ac_val = new Vari("%GC_AddClassType", "%addclass");
            var countv = new Vari("i32", "0");
            var name = "client" + LLVM.n++;
            var strv = new StrV("@" + name, name, name.Length * 1);
            local.llvm.strs.Add(strv);
            var typ = new Vari("i32", (this.n = local.llvm.cn++).ToString());
            var tnp = new Vari("i32", "%tnp" + LLVM.n++);
            local.llvm.main.comps.Add(new Load(tnp, new Vari("i32*", "@cnp")));
            var tv = new Vari("i32", "%cv" + LLVM.n++);
            local.llvm.main.comps.Add(new Add(tv, tnp, typ));
            var ac_call = new Call(null, ac_val, thgc3, tv, countv, strv, new Vari("%GCCheckFuncType", checkname), new Vari("%GCFinalizeFuncType", "null"));
            local.llvm.main.comps.Add(ac_call);

            var rn6 = new Vari("%RootNodeType*", "%rn");
            var block = new Vari("i8*", "%block");
            var decname = "@" + this.letter.name + "dec";
            var funcdec = new Func(local.llvm, new Vari(model + "*", decname), rn6, block);
            local.llvm.comps.Add(funcdec);
            var thgc6 = new Vari("%ThreadGCType*", "%thgc");
            funcdec.comps.Add(new Load(thgc6, new Vari("%ThreadGCType**", "@thgcp")));

            var gmvari = new Vari("%GC_mallocType", "%gm");
            var gmload = new Load(gmvari, new Vari("%GC_mallocType*", "@GC_malloc"));
            funcdec.comps.Add(gmload);

            var go_v = new Vari("%GCObjectPtr", "%obj" + LLVM.n++);
            var tmp = new Vari("i32", this.n.ToString());
            var tmi = new Vari("i32", "%tmi" + LLVM.n++);
            funcdec.comps.Add(new Load(tmi, new Vari("i32*", "@cnp")));
            var tmv = new Vari("i32", "%tmv" + LLVM.n++);
            funcdec.comps.Add(new Add(tmv, tmi, tmp));
            var go_call = new Call(go_v, gmvari, thgc6, tmv);
            funcdec.comps.Add(go_call);
            funcdec.comps.Add(new Store(new Vari("ptr", "@client"), go_v));
            var go_c2 = new Vari(this.model + "*", "%v" + LLVM.n++);
            var go_cast = new Bitcast(go_c2, go_v);
            funcdec.comps.Add(go_cast);

            var vc = new Vari("i8*", "%v" + LLVM.n++);
            gete = new Gete(this.model, vc, go_c2, new Vari("i32", "0"), new Vari("i32", "0"));
            funcdec.comps.Add(gete);
            store = new Store(vc, block);
            funcdec.comps.Add(store);

            var objptr3 = new Vari(this.model + "**", "%v" + LLVM.n++);
            var alloca3 = new Alloca(objptr3);
            funcdec.comps.Add(alloca3);
            var objstore3 = new Store(objptr3, go_c2);
            funcdec.comps.Add(objstore3);

            srcall = new Call(null, srv, thgc6, objptr3);

            int count = 8;
            var blk0 = draw.children[0] as Block;
            int order = 1;
            var blk2 = draw.children[1] as Block;
            var decs = new List<Obj>();
            foreach (var kv in blk0.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                    }
                    else if (variable.cls == local.Bool)
                    {
                        var vdraw = new Vari("i1", "%v" + LLVM.n++);
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        var vdraw = new Vari(variable.cls.model + "*", "%v" + LLVM.n++);
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));


                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), block, new Vari("i8*", "@" + f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);
                }
                else if (v.type == ObjType.SignalFunction)
                {
                    var f = v as SignalFunction;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%MemFuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%MemFuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%MemFuncType*", "@" + f.letter.name + "dec" + f.identity), new Vari("%RootNodeType*", "%rn"), block);
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%MemFuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);
                }
            }
            foreach (var kv in blk2.vmapA)
            {
                var v = kv.Value;
                if (v.type == ObjType.Variable)
                {
                    var variable = v as Variable;
                    variable.order = order++;
                    if (variable.cls.type == ObjType.Var) variable.cls = (variable.cls as Var).cls;
                    if (variable.cls == local.Int)
                    {
                        typedec.comps.Add(new TypeVal("i32", kv.Key));
                        count += 8;
                    }
                    else if (variable.cls == local.Bool)
                    {
                        typedec.comps.Add(new TypeVal("i1", kv.Key));
                        count += 8;
                    }
                    else
                    {
                        if (variable.cls.identity == 0) variable.cls.identity = LLVM.n++;
                        typedec.comps.Add(new TypeVal(variable.cls.model + "*", kv.Key));
                        count += 8;

                        vari = new Vari("i8*", "%v" + LLVM.n++);
                        gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", variable.order.ToString()));
                        funccheck.comps.Add(gete);
                        vari2 = new Vari("i8*", "%v" + LLVM.n++);
                        co_lod = new Load(vari2, vari);
                        funccheck.comps.Add(co_lod);
                        co_cval = new Vari("i8*", "%v" + LLVM.n++);
                        co_call = new Call(co_cval, co_val, thgc2, vari);
                        funccheck.comps.Add(co_call);
                        store = new Store(vari, co_cval);
                        funccheck.comps.Add(store);
                    }
                }
                else if (v.type == ObjType.Function)
                {
                    var f = v as Function;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);

                }
                else if (v.type == ObjType.ClassObj)
                {
                    var f = v as ClassObj;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    decs.Add(f);
                    f.order = order++;
                    typedec.comps.Add(new TypeVal("%FuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%FuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%FuncType*", "@" + f.letter.name + "dec" + f.identity), go_v, new Vari("i8*", f.drawcall), new Vari("i8*", f.call));
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%FuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);
                }
                else if (v.type == ObjType.SignalFunction)
                {
                    var f = v as SignalFunction;
                    if (f.identity == 0) f.identity = LLVM.n++;
                    f.order = order++;
                    decs.Add(f);
                    typedec.comps.Add(new TypeVal("%MemFuncType*", kv.Key));

                    vari = new Vari("i8*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, vari, i8p, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funccheck.comps.Add(gete);
                    vari2 = new Vari("i8*", "%v" + LLVM.n++);
                    co_lod = new Load(vari2, vari);
                    funccheck.comps.Add(co_lod);
                    co_cval = new Vari("i8*", "%v" + LLVM.n++);
                    co_call = new Call(co_cval, co_val, thgc2, vari);
                    funccheck.comps.Add(co_call);
                    store = new Store(vari, co_cval);
                    funccheck.comps.Add(store);
                    count += 8;

                    var va0 = new Vari("%MemFuncType*", "%v" + LLVM.n++);
                    var fu_call = new Call(va0, new Vari("%MemFuncType*", "@" + f.letter.name + "dec" + f.identity), new Vari("%RootNodeType*", "%rn"), block);
                    funcdec.comps.Add(fu_call);
                    var va2 = new Vari(this.model + "*", "%v" + LLVM.n++);
                    funcdec.comps.Add(new Load(va2, objptr3));
                    var va = new Vari("%MemFuncType*", "%v" + LLVM.n++);
                    gete = new Gete(this.model, va, va2, new Vari("i32", "0"), new Vari("i32", f.order.ToString()));
                    funcdec.comps.Add(gete);
                    var fu_store = new Store(va, va0);
                    funcdec.comps.Add(fu_store);
                }
            }
            for (var i = 0; i < decs.Count; i++) decs[i].Dec(local);
            local.llvm.funcs.Add(funcdec);
            this.bas = objptr3;
            local.blocks.Add(blk0);
            blk0.exeC(local);
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
            local.blocks.Add(blk2);
            funccheck.comps.Add(new Ret(new Vari("void", null)));

            var rnpv = new Vari("i32*", "%ptr");
            funcdec.comps.Add(new Gete("%RootNodeType", rnpv, rn6, new Vari("i32", "0"), new Vari("i32", "1")));
            var rnpv12 = new Vari("i32", "%v");
            var rnp12load = new Load(rnpv12, rnpv);
            funcdec.comps.Add(rnp12load);
            var rnpv2 = new Vari("i32", "%dec");
            var rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
            funcdec.comps.Add(rnpsub);
            funcdec.comps.Add(new Store(rnpv, rnpv2));
            funcdec.comps.Add(new Ret(go_c2));

            this.bas = objptr;
            local.labs.Add(new Lab("entry"));
            blk2.exeC(local);
            local.labs.RemoveAt(local.labs.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);
            local.blocks.RemoveAt(local.blocks.Count - 1);

            rnpv = new Vari("i32*", "%ptr");
            local.llvm.func.comps.Add(new Gete("%RootNodeType", rnpv, rn4, new Vari("i32", "0"), new Vari("i32", "1")));
            rnpv12 = new Vari("i32", "%v");
            rnp12load = new Load(rnpv12, rnpv);
            local.llvm.func.comps.Add(rnp12load);
            rnpv2 = new Vari("i32", "%dec");
            rnpsub = new Sub(rnpv2, rnpv12, new Vari("i32", "1"));
            local.llvm.func.comps.Add(rnpsub);
            local.llvm.func.comps.Add(new Store(rnpv, rnpv2));

            local.llvm.func.comps.Add(new Ret(new Vari("void", null)));
            local.llvm.funcs.RemoveAt(local.llvm.funcs.Count - 1);
            var go_v4 = new Vari(model + "*", "%v" + LLVM.n++);
            var v7 = new Vari("%GCObjectPtr", "%v" + LLVM.n++);
            if (local.blocks.Last().obj.obj.type == ObjType.IfBlock) local.llvm.func.comps.Add(new Load(v7, (local.blocks.Last().obj.obj as IfBlock).iflabel.bas));
            else local.llvm.func.comps.Add(new Load(v7, local.blocks.Last().obj.obj.bas));
            var fu_call2 = new Call(go_v4, new Vari(model + "*", "@" + letter.name + "dec"), rn4, v7);
            local.llvm.func.comps.Add(fu_call2);
            local.llvm.func.comps.Add(new Call(null, new Vari(model + "*", call), rn4, go_v4));

        }
    }
    partial class Connect
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    var word = val2 as Word;
                    n++;
                    val2 = primary.children[n];
                    if (word.name == "new")
                    {
                        if (val2.type == ObjType.Bracket)
                        {
                            var val = val2.exeC(local).GetterC(local);
                            if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                            var blk1 = val as Block;
                            if (blk1.rets.Count == 1)
                            {
                                //create MemTable
                                var mt = new Vari("%CreateMemTableType", "%mt" + LLVM.n++);
                                local.llvm.func.comps.Add(new Load(mt, new Vari("%CreateMemTableType*", "@CreateMemTable")));
                                var mv = new Vari("%MemTableType*", "%mv" + LLVM.n++);
                                local.llvm.func.comps.Add(new Call(mv, mt));
                                vari = mv;
                                if (blk1.rets[0].type == ObjType.ServerClient) return new ConnectStock(local.sigmapA[(blk1.rets[0] as ServerClient).name] as ServerFunction);
                                else if (blk1.rets[0].type == ObjType.SignalFunction) return new ConnectStock(blk1.rets[0] as SignalFunction);
                                else return Obj.Error(ObjType.Error, blk1.letters[0], "connect.newはアドレスしか引数にとりません");
                            }
                            else Obj.Error(ObjType.Error, blk1.letter, "connect.newの引数は0か1です");
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "connect.newの()が宣言されていません");
                    }
                    else return Obj.Error(ObjType.Error, word.letter, "connectに" + word.name + "は宣言されていません");
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の後には名前がきます");
            }
            return Obj.Error(ObjType.Error, val2.letter, ".が来ていません");
        }
    }
    partial class ConnectStock
    {
        SignalFunction value;
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    var word = val2 as Word;
                    n++;
                    val2 = primary.children[n];
                    if (word.name == "back")
                    {
                        var variable = new Value(new AddressType());
                        //memtable.back = coroutine
                        return variable;
                    }
                    else if (word.name == "Store")
                    {
                        if (val2.type == ObjType.Bracket)
                        {
                            var val = val2.exeC(local).GetterC(local);
                            if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                            var block = val as Block;
                            for (var i = 0; i < block.rets.Count; i++)
                            {
                                if (block.rets[i].type == ObjType.ModelValue || block.rets[i].type == ObjType.GeneValue)
                                {
                                    //memtable.Store(modelvalue)
                                }
                                else return Obj.Error(ObjType.Error, block.letters[0], "Storeできるのはmodel変数かgene変数だけです");
                            }
                            n++;
                            return new VoiVal();
                        }
                        else return Obj.Error(ObjType.NG, val2.letter, "Storeの()が宣言されていません");
                    }
                    else
                    {
                        var insblocks = local.blocks;
                        local.blocks = (value.draw.children[0] as Block).blocks;
                        var v = local.getB(word.name, word.letter);
                        local.blocks = insblocks;
                        //obj.x
                        if (v.type == ObjType.ServerFunction || v.type == ObjType.SignalFunction) return new ConnectStock(v as SignalFunction);
                        else return v;
                    }
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の後には名前がきます");
            }
            else if (val2.type == ObjType.Block)
            {
                var blk01 = val2.exeC(local).GetterC(local) as Block;
                var val1 = blk01.rets[0];
                if (val1 is not ModelObj) return Obj.Error(ObjType.Error, val1.letter, "ModelクラスかGeneクラスを指定してください"); ;
                var modelobj = val1 as ModelObj;
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Dot)
                {
                    n++;
                    val2 = primary.children[n];
                    if (val2.type == ObjType.Word)
                    {
                        var word2 = val2 as Word;
                        n++;
                        val2 = primary.children[n];
                        if (word2.name == "Output")
                        {
                            if (val2.type == ObjType.Bracket)
                            {
                                if (val2.children.Count == 0)
                                {
                                    //memtable.Output()
                                    return new ElemObj(local, local.getA("sheet") as ElemType, null);
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "Outputの引数は0です");
                            }
                            else return Obj.Error(ObjType.NG, val2.letter, "Outputの()がありません");
                        }
                        else if (word2.name == "Select")
                        {
                            if (val2.type == ObjType.Bracket)
                            {
                                //memtable.Select(func)
                                var val = val2.exeC(local).GetterC(local);
                                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                                var blk0 = val as Block;
                                if (blk0.rets.Count != 1) return Obj.Error(ObjType.Error, val2.letter, "Selectの引数は1です");
                                if (blk0.rets[0].type != ObjType.Function) return Obj.Error(ObjType.Error, blk0.letters[0], "Selectの引数は()でなければなりまえん");
                                var func = blk0.rets[0] as Function;
                                var blk = func.draw.children[0] as Block;
                                if (blk.vmapA.Count != 1) return Obj.Error(ObjType.Error, val2.letter, "Selectの関数の引数は1です");
                                var ftype = new FuncType(local.Bool);
                                ftype.draws.Add(modelobj);
                                var ret = TypeCheck.CheckCVB(ftype, func, CheckType.Setter, local);
                                if (ret.type == ObjType.Wait || ret.type == ObjType.Error || ret.type == ObjType.NG) return ret;
                                return blk;
                            }
                            else return Obj.Error(ObjType.NG, val2.letter, "Selectの()がありません");
                        }
                        else if (word2.name == "First")
                        {
                            if (val2.type == ObjType.Bracket)
                            {
                                //memtable.First()
                                if (val2.children.Count == 0)
                                {
                                    return Value.New(modelobj, local, word2.letter);
                                }
                                else return Obj.Error(ObjType.Error, val2.letter, "Firstの引数は0です");
                            }
                            else return Obj.Error(ObjType.NG, val2.letter, "Firstの()がありません");
                        }
                        else return Obj.Error(ObjType.Error, word2.letter, modelobj.letter.name + "には" + word2.name + "が宣言されていません");
                    }
                    else return Obj.Error(ObjType.Error, val2.letter, ".の後には名前がきます");
                }
                else return Obj.Error(ObjType.NG, val2.letter, ".が予測されます");
            }
            return Obj.Error(ObjType.NG, val2.letter, ".が予測されます");
        }
    }
    partial class AddressType
    {
    }
    partial class Address
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Dot)
            {
                n++;
                val2 = primary.children[n];
                if (val2.type == ObjType.Word)
                {
                    if (val2.letter == local.letter && local.kouhos == null)
                    {
                        local.kouhos = new SortedList<string, Obj>();
                    }
                    var word = val2 as Word;
                    n++;
                    if (val2.type == ObjType.Dot)
                    {
                        n++;
                        val2 = primary.children[n];
                        if (val2.type == ObjType.ServerClient)
                        {
                            var sc = val2 as ServerClient;
                            var p = local.panel.form.LoadProjectC(address + "." + word.name, sc.name);
                            if (sc.name == "server")
                            {
                            }
                            else if (sc.name == "client")
                            {
                                //load dll
                            }
                            n++;
                            return (p.children[0] as Block).vmapA[sc.name];
                        }
                        else
                        {
                            return Obj.Error(ObjType.Error, val2.letter, "addressの後ろにはserverかclientがきます");
                        }
                    }
                }
                else return Obj.Error(ObjType.Error, val2.letter, ".の後には名前がきます");
            }
            return Obj.Error(ObjType.NG, val2.letter, "addressの後ろには.か()がきます");
        }
    }
    partial class Mountain
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            n++;
            var val2 = primary.children[n];
            var rets = new List<List<int>>();
        head:
            if (val2.type == ObjType.Word)
            {
                var word = val2 as Word;
                var num = true;
                var n2 = word.name.Length;
                List<int> ret = new List<int>();
                for (var i = word.name.Length - 1; i >= 0; i--)
                {
                    if (num)
                    {
                        if ('0' <= word.name[i] && word.name[i] <= '9')
                        {
                            if (!num)
                            {
                                var n3 = 0;
                                for (var j = i + 1; j < n2; j++)
                                {
                                    n3 *= ('Z' - 'A' + 1);
                                    n3 += word.name[j] - 'A' + 1;
                                }
                                ret.Add(n3);
                            }
                        }
                        else if ('A' <= word.name[i] && word.name[i] <= 'Z')
                        {
                            if (num)
                            {
                                var text = word.name.Substring(i + 1, n2 - i - 1);
                                ret.Add(Convert.ToInt32(text) - 1);
                                n2 = i + 1;
                                num = !num;
                            }
                        }
                        else return Obj.Error(ObjType.Error, val2.letter, "範囲の記述方式が間違っています");
                    }
                }
                if (num)
                {
                    var text = word.name.Substring(0, n2);
                    ret.Add(Convert.ToInt32(text) - 1);
                }
                else
                {
                    var n3 = 0;
                    for (var j = 0; j < n2; j++)
                    {
                        n3 *= ('Z' - 'A' + 2);
                        n3 += word.name[j] - 'A' + 1;
                    }
                    ret.Add(n3 - 1);
                }
                n++;
                val2 = primary.children[n];
                rets.Add(ret);
                if (rets.Count == 1)
                {
                    if (val2.type == ObjType.Mountain)
                    {
                        n++;
                        val2 = primary.children[n];
                        goto head;
                    }
                    var blk = new Block(ObjType.Array);
                    var blk1 = new Block(ObjType.Array);
                    blk.rets.Add(blk1);
                    blk.rets.Add(blk1);
                    for (var i = 0; i < rets[0].Count; i++)
                    {
                        blk1.rets.Add(new Number(rets[0][i]) { cls = local.Int });
                    }
                    return blk;
                }
                else
                {
                    n--;
                    var blk = new Block(ObjType.Array);
                    var blk1 = new Block(ObjType.Array);
                    blk.rets.Add(blk1);
                    var blk2 = new Block(ObjType.Array);
                    blk.rets.Add(blk2);
                    for (var i = 0; i < rets[0].Count; i++)
                    {
                        blk1.rets.Add(new Number(rets[0][i]) { cls = local.Int });
                    }
                    for (var i = 0; i < rets[1].Count; i++)
                    {
                        blk2.rets.Add(new Number(rets[1][i]) { cls = local.Int });
                    }
                    return blk;
                }
            }
            return Obj.Error(ObjType.Error, val2.letter, "範囲の記述式がきていません");
        }
    }
    partial class Sum
    {
        public override Obj PrimaryC(ref int n, Local local, Primary primary, Obj val2)
        {
            if (val2.type == ObjType.Bracket)
            {
                n++;
                local.calls.Add(local.KouhoSet2);
                var val = val2.exeB(local).GetterB(local);
                local.calls.RemoveAt(local.calls.Count - 1);
                if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                var blk = val as Block;
                var rets = new List<List<int>>();
                if (blk.rets.Count == 1)
                {
                    if (blk.rets[0].type == ObjType.Array)
                    {
                        val = blk.rets[0].exeB(local).GetterB(local);
                        if (val.type == ObjType.Wait || val.type == ObjType.Error || val.type == ObjType.NG) return val;
                        var blk2 = val as Block;
                        for (var i = 0; i < blk2.rets.Count; i++)
                        {
                            rets.Add(new List<int>());
                            foreach (var nums in (blk2.rets[i].Clone().GetterB(local) as Block).rets)
                            {
                                if (nums.type == ObjType.Number) rets[i].Add((nums as Number).value);
                                else return Obj.Error(ObjType.Error, val2.letter, "範囲の選択はint[]でなければあなりません");
                            }
                        }
                    }
                    else return Obj.Error(ObjType.Error, blk.letters[0], "Sumの引数はint[]です。");
                }
                else return Obj.Error(ObjType.Error, val2.letter, "Sumの引数は1です");
                if (rets[0].Count < rets[1].Count)
                {
                    for (var i = rets[0].Count; i < rets[1].Count; i++) rets[0].Add(0);
                }
                else if (rets[0].Count > rets[1].Count)
                {
                    for (var i = rets[1].Count; i < rets[0].Count; i++) rets[1].Add(0);
                }
                var minmax = new List<List<int>>();
                minmax.Add(new List<int>());
                minmax.Add(new List<int>());
                for (var i = 0; i < rets[0].Count; i++)
                {
                    if (rets[0][i] <= rets[1][i])
                    {
                        minmax[0].Add(rets[0][i]);
                        minmax[1].Add(rets[1][i]);
                    }
                    else
                    {
                        minmax[1].Add(rets[0][i]);
                        minmax[0].Add(rets[1][i]);
                    }
                }
                int[] array = new int[minmax.Count];
                var n2 = 0;
                ObjInt c = new ObjInt();
                List<Cell> cells = new List<Cell>();
                SetArrayC((nums) =>
                {
                    c.count++;
                    var text = sheet.cells[nums[0]][nums[1]].Text3(local).Trim();
                    if (text != "") n2 += Convert.ToInt32(text);
                    cells.Add(sheet.cells[nums[0]][nums[1]]);
                }, array, minmax, 0, local);
                return new Number(n2) { cls = local.Int };
            }
            return Obj.Error(ObjType.NG, val2.letter, "Sumの()がありません");
        }
        void SetArrayC(Action<int[]> task, int[] nums, List<List<int>> minimax, int n, Local local)
        {
            if (n == minimax[0].Count)
            {
                task(nums);
                return;
            }
            for (var i = minimax[0][n]; i <= minimax[1][n]; i++)
            {
                nums[n] = i;
                SetArrayC(task, nums, minimax, n + 1, local);
            }
        }
    }
    partial class Question
    {
        public override Obj exepC(ref int n, Local local, Primary primary)
        {
            var blk = new Block(ObjType.Array);
            var blk1 = new Block(ObjType.Array);
            blk.rets.Add(blk1);
            blk.rets.Add(blk1);
            for (var i = 0; i < 2; i++)
            {
                blk1.rets.Add(new Number(0) { cls = local.Int });
            }
            return blk1;
        }
    }
}
