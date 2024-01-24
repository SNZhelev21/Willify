import {
    LOCAL_STORAGE_ACCESS_TOKEN_KEY,
    SESSION_STORAGE_USER_INFO_KEY
} from "../shared/constants"

import { UserVM } from "../models/user-vm";

class StorageService {
    public retrieveAccessToken(): string | null {
        try {
            const accessToken = localStorage.getItem(LOCAL_STORAGE_ACCESS_TOKEN_KEY);

            if (!accessToken) {
                return null;
            }

            return accessToken;
        }
        catch (e) {
            localStorage.removeItem(LOCAL_STORAGE_ACCESS_TOKEN_KEY);
            return null;
        }
    }

    public saveAccessToken(accessToken: string | null): void {
        if (accessToken) {
            localStorage.setItem(LOCAL_STORAGE_ACCESS_TOKEN_KEY, accessToken);
        }
        else {
            localStorage.removeItem(LOCAL_STORAGE_ACCESS_TOKEN_KEY);
        }
    }

    public saveUserInfo(user: UserVM | null) {
        if (user) {
            sessionStorage.setItem(SESSION_STORAGE_USER_INFO_KEY, JSON.stringify(user));
        }
        else {
            sessionStorage.removeItem(SESSION_STORAGE_USER_INFO_KEY);
        }
    }

    public retrieveUserInfo(): UserVM | null {
        try {
            const userInfo = sessionStorage.getItem(SESSION_STORAGE_USER_INFO_KEY);

            if (!userInfo) {
                return null;
            }

            return JSON.parse(userInfo);
        }
        catch (e) {
            sessionStorage.removeItem(SESSION_STORAGE_USER_INFO_KEY);
            return null;
        }
    }
}

const storageService = new StorageService();
export default storageService;