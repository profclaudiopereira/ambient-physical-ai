#!/usr/bin/env python3

def build_context(identity_package):
    profile = identity_package.get("profile", {})
    nfc = identity_package.get("nfc", {})

    return {
        "active_user": profile.get("name", "Unknown"),
        "role": profile.get("role", "unknown"),
        "environment_context": identity_package.get("context", "Unknown"),
        "uid": nfc.get("uid", "unknown"),
        "source": "identity_node",
    }


def build_human_message(context):
    user = context.get("active_user", "Unknown")
    role = context.get("role", "unknown")
    env = context.get("environment_context", "Unknown")

    if role == "owner" or user.lower() == "claudio":
        greeting = f"Olá {user}, bem-vindo ao laboratório."
    elif role == "learner" or user.lower() == "student":
        greeting = f"Olá {user}, bem-vindo à demonstração."
    else:
        greeting = f"Olá {user}, bem-vindo ao ambiente."

    return f"{greeting}\nContexto: {env}."
