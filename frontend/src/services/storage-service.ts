import {
    LOCAL_STORAGE_ACCESS_TOKEN_KEY
} from "../shared/constants"

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
}

const storageService = new StorageService();
export default storageService;