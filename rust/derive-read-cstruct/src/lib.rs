extern crate proc_macro;
extern crate syn;
#[macro_use]
extern crate quote;

use proc_macro::TokenStream;

#[proc_macro_derive(ReadCStruct)]
pub fn command_parser(input: TokenStream) -> TokenStream {
    let s = input.to_string();
    let ast = syn::parse_derive_input(&s).unwrap();
    let gen = impl_parser(&ast);
    gen.parse().unwrap()
}

fn impl_parser(ast: &syn::DeriveInput) -> quote::Tokens {
    let name = &ast.ident;
    let body = &ast.body;
    let mut fields1 = vec![];
    let mut fields2 = vec![];
    let mut tys = vec![];
    if let &syn::Body::Struct(ref variant) = body {
        for field in variant.fields() {
            if let Some(ref idt) = field.ident {
                fields1.push(idt);
                fields2.push(idt);
            }
            if let syn::Ty::Path(_, ref path) = field.ty {
                for seg in &path.segments {
                    let idt = &seg.ident;
                    let method = syn::Ident::new(format!("le_{}", idt));
                    tys.push(method);
                }
            }
        }
    } else {
        panic!("accepts only structs");
    }
    quote! {
        impl ReadCStruct for #name {
            type Item = #name;
            fn parse(input: &[u8]) -> IResult<&[u8], Self::Item> {
                do_parse!(input,
                    #(#fields1: #tys)>>*>>
                    (#name{#(#fields2),*})
                )
            }
        }
    }
}
